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
    
    if (fs::exists(EXT_SDCARD_BASE_PATH)) {
        items->push_back(MenuItem(ACTION_BOOT_SDCARD, "Boot from external SD card"));
    }
}

void MainMenu::onItemSelected(int action) {
    if (action == ACTION_BOOT_INTERNAL) {
        boot_target("internal");
    } else if (action == ACTION_BOOT_SDCARD) {
        switchMenu(make_shared<ImageSelectionMenu>("Choose image from SD card",
            EXT_SDCARD_BASE_PATH));
    }
}

string ImageSelectionMenu::getTitle() {
    return title;
}

void ImageSelectionMenu::scanImages() {
    images = vector<string>();
    
    if (fs::exists(base_path / fs::path("system.img"))) {
        images.push_back("system.img");
    }
    
    // TODO: Actually scan the subdirectories
}

void ImageSelectionMenu::populateItems() {
    items->push_back(MenuItem(ACTION_BACK, "..."));
    
    scanImages();
    
    for (int i = 0; i < images.size(); i++) {
        items->push_back(MenuItem(ACTION_IMAGE_BASE + i, images.at(i)));
    }
}

void ImageSelectionMenu::onItemSelected(int action) {
    if (action == ACTION_BACK) {
        switchMenu(main_menu);
    } else if (action >= ACTION_IMAGE_BASE) {
        int img_idx = action - ACTION_IMAGE_BASE;
        auto path = base_path / fs::path(images.at(img_idx));
        boot_target(path.string());
    }
}
