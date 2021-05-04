#include "ui.h"
#include <iostream>
#include <sstream>
#include <string>
#include <minui/minui.h>
#include <unistd.h>

using namespace std;

// `target` can be "internal", which means the system_phh internal partition,
// or path to the system image file. See the `boot-target` script for details
void boot_target(const char *target) {
    // Release the UI resources because we are handing over to the real init
    UI::exit();
    execl("/bin/sh", "sh", "/bin/boot-target", target, (char *) NULL);
}

#define ACTION_BOOT_INTERNAL 1001
#define ACTION_BOOT_SDCARD   1002

class MainMenu : public UI::Menu {
private:
    shared_ptr<vector<UI::MenuItem>> items;
public:
    MainMenu() {
        items = make_shared<vector<UI::MenuItem>>();
        items->push_back(UI::MenuItem(ACTION_BOOT_INTERNAL, "Boot from internal storage"));
        items->push_back(UI::MenuItem(ACTION_BOOT_SDCARD, "Boot from external SD card"));
    }
    
    string getTitle() {
        return "Boot Diverter";
    }
    
    shared_ptr<vector<UI::MenuItem>> getItems() {
        return items;
    }
    
    void onItemSelected(int action) {
        if (action == ACTION_BOOT_INTERNAL) {
            boot_target("internal");
        }
    }
};

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
