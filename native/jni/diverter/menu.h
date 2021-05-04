#pragma once
#include "ui.h"
#include <ctime>
#include <filesystem>

using namespace std;
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
    time_t start_time;
public:
    MainMenu();
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
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
};
