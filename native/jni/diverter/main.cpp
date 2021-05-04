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

class MainMenu : public UI::Menu {
private:
    shared_ptr<vector<string>> items;
public:
    MainMenu() {
        items = make_shared<vector<string>>();
        items->push_back("Boot from internal storage");
        items->push_back("Boot from external SD card");
    }
    
    string getTitle() {
        return "Boot Diverter";
    }
    
    shared_ptr<vector<string>> getItems() {
        return items;
    }
    
    void onItemSelected(int id) {
        if (id == 0) {
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
