#pragma once
#include "ui.h"

using namespace std;
using namespace UI;

#define ACTION_BOOT_INTERNAL 1001
#define ACTION_BOOT_SDCARD   1002
class MainMenu : public Menu {
private:
    shared_ptr<vector<MenuItem>> items;
public:
    MainMenu();
    string getTitle();
    shared_ptr<vector<MenuItem>> getItems();
    void onItemSelected(int action);
};
