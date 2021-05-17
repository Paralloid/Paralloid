#include "main.h"
#include "menu.h"
#include <fstream>

// Menu instances
shared_ptr<UI::Menu> main_menu;

int main() {
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
