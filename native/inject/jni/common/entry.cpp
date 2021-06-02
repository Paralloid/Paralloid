#define LOG_TAG "hooked_entry"
#include <inject/common.h>
#include <cstdlib>
#include <errno.h>
#include <filesystem>
#include <string>
#include <unistd.h>

using namespace std;

extern char **environ;

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
    string hook_lib = "/debug_ramdisk/dev/mntRAM/hook/lib" + progname + "_hook.so";
    string real_binary = "/debug_ramdisk/dev/mntRAM/bin/" + progname;
    LOGI("Loading LD_PRELOAD hook: %s", hook_lib.c_str());
    LOGI("Loading original binary: %s", real_binary.c_str());
    
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
