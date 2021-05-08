#pragma once
#include "ui.h"
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#define INTERNAL_SYSTEM_PATH "/dev/block/mapper/system_phh"
#define EXT_SDCARD_BASE_PATH "/dev/mntSD/Diverter"
#define USERDATA_BASE_PATH   "/dev/mntDATA/unencrypted/Diverter"

using namespace std;

extern shared_ptr<UI::Menu> main_menu;

// `target` can be "internal", which means the system_phh internal partition,
// or path to the system image file. See the `boot-target` script for details
void boot_target(string target);

// A "system" command but more sane and secure
template<typename... Args>
void fork_execl(Args... args) {
    int pid, status;
    if ((pid = fork()) == 0) {
        execl(args..., nullptr);
    } else {
        waitpid(pid, &status, 0);
    }
}

// Calls the /bin/format-userdata-image script
// `path` should be full path to the userdata.img
void format_userdata_image(string path);
