#include "menu.h"
#include "main.h"

shared_ptr<vector<MenuItem>> CachedMenu::getItems() {
    if (items == nullptr) {
        items = make_shared<vector<MenuItem>>();
        populateItems();
    }
    return items;
}

string MainMenu::getTitle() {
    return "Boot Diverter";
}

void MainMenu::populateItems() {
    items->push_back(MenuItem(ACTION_BOOT_INTERNAL, "Boot from internal storage"));
    items->push_back(MenuItem(ACTION_BOOT_SDCARD, "Boot from external SD card"));
}

void MainMenu::onItemSelected(int action) {
    if (action == ACTION_BOOT_INTERNAL) {
        boot_target("internal");
    }
}
