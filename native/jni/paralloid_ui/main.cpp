#include "main.h"
#include <fstream>
#include <signal.h>

// Menu instances
shared_ptr<CachedMenu> main_menu;

// Update the menu on SIGHUP
// This is used to signal sdcard being mounted
static void sighup_handler(int _signo) {
    // Only reset the cache of the main menu as it is the only one
    // that is concerned with the mount state of the sdcard
    // Other menus related to sdcard cannot be loaded unless the main menu
    // actually shows the sdcard option
    main_menu->resetCache();
    // Force refresh the menu regardless of the current displayed menu
    // If we are not on main_menu right now, this will simply do nothing
    UI::refreshMenu();
}

int main() {
    // Setup the SIGHUP handler
    signal(SIGHUP, &sighup_handler);
    
    // Initialize the menus
    main_menu = make_shared<MainMenu>();
    
    UI::init();
    UI::switchMenu(main_menu);
    UI::runForever();
    UI::exit();
    return 0;
}

void boot_target(string target) {
    // Release the UI resources because we are handing over to the real init
    // Also, construct a user-friendly boot message to display before the
    // system takes over the framebuffer
    UI::exit("Booting target " + target + "...");
    // Notify the init process to proceed with booting
    // by writing the target to a marker file
    createMarkerFile("/dev/.paralloid_boot_target", target);
    // Wait a while before exiting the process
    // The purpose is to hold the DRM device node fd for a while, because
    // closing the fd will prevent the touchscreen from initializing on
    // some devices (e.g. Motorola G50). In our minui fork, we have replaced
    // close(drm_fd) with dropDrmMaster(drm_fd), and here we just need to
    // keep the process around for a few moments before hwcomposer comes up.
    sleep(60);
    exit(0);
}

void format_userdata_image(string path) {
    fork_execl("/bin/sh", "sh", "/bin/format-userdata-image", path.c_str());
}
