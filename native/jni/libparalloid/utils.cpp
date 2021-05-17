#include <paralloid/utils.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <sys/syscall.h>
#include <sys/reboot.h>
#include <unistd.h>

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
} 

void rebootInto(const char* target) {
    sync();
    
    if (target == nullptr) {
        reboot(RB_AUTOBOOT);
    } else {
        syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                LINUX_REBOOT_CMD_RESTART2, target);
    }
}

void createMarkerFile(std::string file, std::optional<std::string> content) {
    // To ensure atomicity, we first create a temporary file, then rename it after writing
    std::ofstream ofs(file + ".tmp");
    if (content != std::nullopt) {
        ofs << *content;
    }
    ofs.close();
    rename((file + ".tmp").c_str(), file.c_str());
}
