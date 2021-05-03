#include "ui.h"
#include <iostream>
#include <minui/minui.h>
#include <unistd.h>

using namespace std;

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
        // TODO
    }
};

// Menu instances
shared_ptr<UI::Menu> main_menu;

int main() {
    // Initialize the menus
    main_menu = make_shared<MainMenu>();
    
    UI::init();
    UI::switchMenu(main_menu);
    
    while (true) {
        sleep(1000);
    }
    
    UI::exit();
    return 0;
}
