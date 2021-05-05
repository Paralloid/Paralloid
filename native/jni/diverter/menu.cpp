#include "menu.h"
#include "main.h"
#include <sstream>
#include <sys/reboot.h>

#define DEFAULT_HELP_TEXT \
    "Use volume up/down and power to select. " \
    "Long-press volume down for extra options." \

void boot_target_with_confirmation(string target, shared_ptr<Menu> current_menu) {
    if (target == "internal") {
        boot_target(target);
        return;
    } else {
        auto path = fs::path(target);
        if (!fs::exists(path.parent_path() / "userdata.img")) {
            switchMenu(make_shared<BootConfirmationMenu>(
                "About to create a userdata image (8 GiB by default) for this boot target.\n"
                "This may not work or can take extremely long "
                "depending on the underlying filesystem.\n"
                "If this fails, you can always create the image manually.\n"
                "Continue?",
                current_menu, path));
        } else {
            boot_target(path.string());
        }
    }
}

shared_ptr<vector<MenuItem>> CachedMenu::getItems() {
    if (items == nullptr) {
        items = make_shared<vector<MenuItem>>();
        populateItems();
    }
    return items;
}

MainMenu::MainMenu() {
    // Only start the countdown timer when MainMenu is
    // created for the first time (there will be only
    // one instance of MainMenu in the program).
    start_time = high_resolution_clock::now();
}

string MainMenu::getTitle() {
    return "Boot Diverter";
}

optional<string> MainMenu::getExtraText() {
    if (remaining_secs > 0) {
        ostringstream s;
        s << "Selecting default option in " << remaining_secs << " seconds...";
        return s.str();
    } else {
        return DEFAULT_HELP_TEXT;
    }
}

void MainMenu::populateItems() {
    if (fs::exists(INTERNAL_SYSTEM_PATH)) {
        items->push_back(MenuItem(ACTION_BOOT_INTERNAL, "Boot from internal storage"));
    }
    
    if (fs::exists(EXT_SDCARD_BASE_PATH)) {
        items->push_back(MenuItem(ACTION_BOOT_SDCARD, "Boot from external SD card"));
    }
    
    items->push_back(MenuItem(ACTION_REBOOT, "Reboot"));
}

void MainMenu::onActiveItemChanged(int idx, int action) {
    // If the user changed selection, get rid of the countdown
    remaining_secs = -1;
    refreshMenu();
}

void MainMenu::onEventLoopIteration() {
    if (remaining_secs == -1) return;
    
    auto now = high_resolution_clock::now();
    remaining_secs = max(0,
        MAIN_MENU_TIMEOUT - (int) (duration_cast<seconds>(now - start_time).count()));
    refreshMenu();
    
    if (remaining_secs == 0) {
        selectCurrentItem();
    }
}

void MainMenu::onItemSelected(int action) {
    remaining_secs = -1;
    
    if (action == ACTION_BOOT_INTERNAL) {
        boot_target("internal");
    } else if (action == ACTION_BOOT_SDCARD) {
        switchMenu(make_shared<ImageSelectionMenu>("Choose image from SD card",
            EXT_SDCARD_BASE_PATH));
    } else if (action == ACTION_REBOOT) {
        reboot(RB_AUTOBOOT);
    }
}

void MainMenu::onItemExtraOptionsSelected(int action) {
    if (action == ACTION_BOOT_INTERNAL) {
        switchMenu(make_shared<ExtraOptionsMenu>("internal", shared_from_this()));
    }
}

string ImageSelectionMenu::getTitle() {
    return title;
}

optional<string> ImageSelectionMenu::getExtraText() {
    return DEFAULT_HELP_TEXT;
}

void ImageSelectionMenu::scanImages() {
    images = vector<string>();
    
    if (fs::exists(base_path / fs::path("system.img"))) {
        images.push_back("system.img");
    }
    
    // Scan the subdirectories
    for (auto& p: fs::directory_iterator(base_path)) {
        if (!p.is_directory()) continue;
        if (!fs::exists(p / fs::path("system.img"))) continue;
        auto relative_path = fs::relative(p / fs::path("system.img"), base_path);
        images.push_back(relative_path.string());
    }
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
        boot_target_with_confirmation(path.string(), shared_from_this());
    }
}

void ImageSelectionMenu::onItemExtraOptionsSelected(int action) {
    if (action < ACTION_IMAGE_BASE) return;
    
    int img_idx = action - ACTION_IMAGE_BASE;
    auto path = base_path / fs::path(images.at(img_idx));
    switchMenu(make_shared<ExtraOptionsMenu>(path.string(), shared_from_this()));
}

string ConfirmationMenu::getTitle() {
    return message;
}

void ConfirmationMenu::populateItems() {
    items->push_back(MenuItem(ACTION_YES, "Yes"));
    items->push_back(MenuItem(ACTION_NO, "No"));
}

void ConfirmationMenu::onItemSelected(int action) {
    if (action == ACTION_YES) {
        onConfirmed();
    } else if (action == ACTION_NO) {
        switchMenu(last_menu);
    }
}

void BootConfirmationMenu::onConfirmed() {
    boot_target(image_path.string());
}

string ExtraOptionsMenu::getTitle() {
    return "Extra options for target " + target;
}

void ExtraOptionsMenu::populateItems() {
    items->push_back(MenuItem(ACTION_BACK, "..."));
    items->push_back(MenuItem(ACTION_ADB_DEBUGGABLE,
        "Boot with forced debuggable mode (ro.adb.secure=0)"));
    
    if (target != "internal") {
        auto path = fs::path(target);
        if (fs::exists(path.parent_path() / "userdata.img")) {
            items->push_back(MenuItem(ACTION_DELETE_DATA, "Delete userdata image"));
        }
    }
}

void ExtraOptionsMenu::onItemSelected(int action) {
    if (action == ACTION_BACK) {
        switchMenu(last_menu);
    } else if (action == ACTION_ADB_DEBUGGABLE) {
        // This will be read from the boot-target script
        setenv("DIVERTER_FORCE_DEBUGGABLE", "1", 1);
        boot_target_with_confirmation(target, shared_from_this());
    } else if (action == ACTION_DELETE_DATA) {
        switchMenu(make_shared<DeleteDataConfirmationMenu>(last_menu, fs::path(target)));
    }
}

void DeleteDataConfirmationMenu::onConfirmed() {
    fs::remove(image_path.parent_path() / "userdata.img");
    switchMenu(last_menu);
}
