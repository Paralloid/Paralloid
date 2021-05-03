#include "ui.h"
#include <iostream>
#include <sstream>
#include <string>
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
        
        // TODO: remove these test
        for (int i = 0; i < 20; i++) {
            ostringstream s;
            s << "Test item " << i;
            items->push_back(s.str());
            if (i == 3) {
                items->push_back("Hello another super duper long item text but this is definitely not long enough and I have to come up with more words to fill this thing");
            }
            if (i == 15) {
                items->push_back("Hello super duper long item text I hope this is long enough");
            }
        }
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
    UI::runForever();
    UI::exit();
    return 0;
}
