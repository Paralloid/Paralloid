#include "menu.h"
#include "main.h"

MainMenu::MainMenu() {
    items = make_shared<vector<MenuItem>>();
    items->push_back(MenuItem(ACTION_BOOT_INTERNAL, "Boot from internal storage"));
    items->push_back(MenuItem(ACTION_BOOT_SDCARD, "Boot from external SD card"));
}

string MainMenu::getTitle() {
    return "Boot Diverter";
}

shared_ptr<vector<MenuItem>> MainMenu::getItems() {
    return items;
}

void MainMenu::onItemSelected(int action) {
    if (action == ACTION_BOOT_INTERNAL) {
        boot_target("internal");
    }
}
