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

void boot_target(const char *target) {
    // Release the UI resources because we are handing over to the real init
    UI::exit();
    execl("/bin/sh", "sh", "/bin/boot-target", target, (char *) NULL);
}
