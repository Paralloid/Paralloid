#include <fcntl.h>
#include <mntent.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <android-base/strings.h>
#include <paralloid/utils.h>

#include <filesystem>
#include <iostream>

using android::base::StartsWith;
using namespace std::literals;
namespace fs = std::filesystem;

// Adapted from platform/system/core/init/switch_root.cpp
std::vector<std::string> GetMountsToMove(const std::string& old_root, const std::string& new_root) {
    auto fp = std::unique_ptr<std::FILE, decltype(&endmntent)>{setmntent("/proc/mounts", "re"),
                                                               endmntent};
    if (fp == nullptr) {
        std::cout << "Failed to open /proc/mounts" << std::endl;
    }
    
    std::vector<std::string> result;
    mntent* mentry;
    while ((mentry = getmntent(fp.get())) != nullptr) {
        // We won't try to move old root.
        if (fs::path(mentry->mnt_dir) == fs::path(old_root)) {
            continue;
        }
        
        // Ignore everything outside old_root
        if (old_root != "/" && !StartsWith(mentry->mnt_dir, old_root + "/")) {
            continue;
        }
        
        // The new root mount is handled separately.
        if (mentry->mnt_dir == new_root) {
            continue;
        }
        
        // Move operates on subtrees, so do not try to move children of other mounts.
        if (std::find_if(result.begin(), result.end(), [&mentry](const auto& older_mount) {
                return StartsWith(mentry->mnt_dir, older_mount + "/");
            }) != result.end()) {
            continue;
        }
        
        result.emplace_back(mentry->mnt_dir);
    }
    
    return result;
}

void NormalizePathString(std::string& p) {
    if (p != "/" && p.at(p.length() - 1) == '/') {
        p.replace(p.length() - 1, 1, "");
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage: move_mount_tree <old_root> <new_root>" << std::endl;
        return -1;
    }
    
    std::vector<std::string> rbind_dirs;
    if (auto _rbind_dirs = std::getenv("MOVE_MOUNT_RBIND_DIRS")) {
        std::string _rbind_dirs_str(_rbind_dirs);
        rbind_dirs = split(_rbind_dirs_str, ';');
    }
    
    std::string old_root(argv[1]);
    std::string new_root(argv[2]);
    NormalizePathString(old_root);
    NormalizePathString(new_root);
    std::cout << "Moving mount tree from " << old_root << " to " << new_root << std::endl;
    
    auto mounts = GetMountsToMove(old_root, new_root);
    for (auto& mnt : mounts) {
        auto new_mount_path = old_root != "/" ?
            std::string(mnt).replace(0, old_root.length(), new_root) : (new_root + mnt);
        std::cout << "Moving " << mnt << " to " << new_mount_path << std::endl;
        bool should_rbind = false;
        for (auto& d : rbind_dirs) {
            if (fs::path(mnt) == fs::path(old_root) / d) {
                should_rbind = true;
                break;
            }
        }
        
        mkdir(new_mount_path.c_str(), 0755);
        if (should_rbind) {
            std::cout << "Using rbind instead of move for " << mnt << std::endl;
            mount(mnt.c_str(), new_mount_path.c_str(), nullptr, MS_BIND | MS_REC, nullptr);
        } else {
            mount(mnt.c_str(), new_mount_path.c_str(), nullptr, MS_MOVE, nullptr);
        }
    }
}
