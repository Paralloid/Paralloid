#include <cstdlib>
#include <errno.h>
#include <unistd.h>

using namespace std;

extern char **environ;

int main(int argc, char *argv[]) {
    setenv("LD_PRELOAD", "/debug_ramdisk/dev/mntRAM/hook/libgatekeeperd_hook.so", 1);
    execve("/debug_ramdisk/dev/mntRAM/bin/gatekeeperd", argv, environ);
    return errno;
}
