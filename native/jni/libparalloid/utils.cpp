#include <paralloid/utils.h>

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
