#include "main.h"
#include "menu.h"
#include <unistd.h>

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
    execl("/bin/sh", "sh", "/bin/boot-target", target.c_str(), (char *) NULL);
}
