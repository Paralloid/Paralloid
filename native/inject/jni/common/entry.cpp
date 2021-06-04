#define LOG_TAG "hooked_entry"
#include <inject/common.h>
#include <cstdlib>
#include <errno.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/mount.h>
#include <unistd.h>

using namespace std;

extern char **environ;

void patch_ld_config() {
    if (filesystem::exists("/linkerconfig/.paralloid_ld_patched")) {
        LOGI("ld.config.txt already patched or now being patched");
        return;
    }
    
    ofstream _tmp("/linkerconfig/.paralloid_ld_patched");
    _tmp.close();
    
    ifstream is("/linkerconfig/ld.config.txt");
    ostringstream oss;
    string line;
    string last_section = "";
    
    while (getline(is, line)) {
        if (line.rfind("[", 0) == 0) {
            if (last_section != "") {
                // We are about to enter a new section; start writing our additional config
                // Allow default namespaces to link to hooks in our ramdisk
                oss << "namespace.default.permitted.paths += /debug_ramdisk/dev/mntRAM/hook" << endl;
                oss << "namespace.default.search.paths += /debug_ramdisk/dev/mntRAM/hook" << endl;
            }
            
            last_section = line;
        }
        
        oss << line << endl;
    }
    
    ofstream os("/linkerconfig/ld.config.txt", ios::trunc);
    os << oss.str() << endl;
}

// Common wrapper for binary hooking that loads a LD_PRELOAD library and executes
// the original binary located in /debug_ramdisk (which should be the Paralloid
// rootfs after our pivot_root operation)
// This binary should be copied for each instance of a binary that we need to
// hook, be applied with the correct SELinux context, and bind-mounted upon
// the original binary. The original binary needs to be backed up in
// /debug_ramdisk/mntRAM/bin/ to be executed, and the LD_PRELOAD libary
// shall be located in /debug_ramdisk/dev/mntRAM/hook/lib<name_of_binary>_hook.so
int main(int argc, char *argv[]) {
    string progname = filesystem::path(string(argv[0])).filename();
    
    if (progname == "app_process64") {
        // app_process has root privileges when started; use it to patch ld.config.txt
        LOGI("Patching ld.config.txt from app_process");
        patch_ld_config();
    }
    
    string hook_lib = "/debug_ramdisk/dev/mntRAM/hook/lib" + progname + "_hook.so";
    
    if (progname == "app_process64" || progname == "app_process32" || progname == "app_process") {
        // app_process has AT_SECURE, so LD_PRELOAD won't work; don't even bother
        // Instead, we injected the library into the dependency table of the original app_process binary
        hook_lib = "";
    }
    
    string real_binary = "/debug_ramdisk/dev/mntRAM/bin/" + progname;
    LOGI("Loading LD_PRELOAD hook: %s", hook_lib.c_str());
    LOGI("Loading original binary: %s", real_binary.c_str());
    
    if (!filesystem::exists(real_binary)) {
        LOGI("Could not find %s, trying to self-umount and execute original binary", real_binary.c_str());
        umount2(argv[0], MNT_DETACH);
        real_binary = string(argv[0]);
    }
    
    char *_preload_orig;
    if ((_preload_orig = getenv("LD_PRELOAD")) != nullptr) {
        // Preserve the original LD_PRELOAD if any
        // This may be used sometimes on some ROMs or in mods
        hook_lib = hook_lib + ":" + _preload_orig;
    }
    
    setenv("LD_PRELOAD", hook_lib.c_str(), 1);
    execve(real_binary.c_str(), argv, environ);
    return errno;
}
