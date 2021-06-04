#define LOG_TAG "app_process_wrapper"
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

// Wrapper over app_process; this is mainly for patching the linker config to allow
// our hooked libraries to be searched / loaded.
int main(int argc, char *argv[]) {
    // app_process has root privileges when started; use it to patch ld.config.txt
    LOGI("Patching ld.config.txt from app_process");
    patch_ld_config();
    
    // Self-unmounting will reveal the real app_process binary; this is because Paralloid uses a bind mount
    umount2(argv[0], MNT_DETACH);
    
    execve(argv[0], argv, environ);
    return errno;
}
