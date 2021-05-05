#pragma once
#include "ui.h"
#include <chrono>
#include <filesystem>

using namespace std;
using namespace std::chrono;
using namespace UI;

namespace fs = filesystem;

class CachedMenu : public Menu {
protected:
    shared_ptr<vector<MenuItem>> items;
    virtual void populateItems() = 0;
public:
    shared_ptr<vector<MenuItem>> getItems();
};

#define ACTION_BACK          1000

#define ACTION_BOOT_INTERNAL 1001
#define ACTION_BOOT_SDCARD   1002
#define ACTION_REBOOT        1100

#define MAIN_MENU_TIMEOUT    10
class MainMenu : public CachedMenu {
private:
    int remaining_secs = MAIN_MENU_TIMEOUT;
    time_point<high_resolution_clock> start_time;
public:
    MainMenu();
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
    void onItemExtraOptionsSelected(int action);
    void onActiveItemChanged(int idx, int action);
    optional<string> getExtraText();
    void onEventLoopIteration();
};

#define ACTION_IMAGE_BASE     1000000
class ImageSelectionMenu : public CachedMenu {
private:
    string title;
    fs::path base_path;
    vector<string> images;
    
    void scanImages();
public:
    ImageSelectionMenu(string title, string base_path)
      : title(title), base_path(fs::path(base_path)) {};
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
    void onItemExtraOptionsSelected(int action);
};

#define ACTION_YES            1001
#define ACTION_NO             1002
class BootConfirmationMenu : public CachedMenu {
private:
    string message;
    fs::path image_path;
public:
    BootConfirmationMenu(string message, fs::path image_path)
      : message(message), image_path(image_path) {};
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
};

#define ACTION_ADB_DEBUGGABLE 1001
class ExtraOptionsMenu : public CachedMenu {
private:
    string target;
    shared_ptr<Menu> last_menu;
public:
    ExtraOptionsMenu(string target, shared_ptr<Menu> last_menu)
      : target(target), last_menu(last_menu) {};
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
};
