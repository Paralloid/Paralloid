#pragma once
#include "ui.h"
#include <private/images.h>

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
#define ACTION_BOOT_USERDATA 1003
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
    vector<BootableImage> images;
    
public:
    ImageSelectionMenu(string title, string base_path)
      : title(title), base_path(fs::path(base_path)) {};
    string getTitle();
    optional<string> getExtraText();
    void populateItems();
    void onItemSelected(int action);
    void onItemExtraOptionsSelected(int action);
};

#define ACTION_YES            1001
#define ACTION_NO             1002
class ConfirmationMenu : public CachedMenu {
private:
    string message;
protected:
    shared_ptr<Menu> last_menu;
    virtual void onConfirmed() = 0;
public:
    ConfirmationMenu(string message, shared_ptr<Menu> last_menu)
      : message(message), last_menu(last_menu) {};
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
};

class BootConfirmationMenu : public ConfirmationMenu {
private:
    fs::path image_path;
public:
    BootConfirmationMenu(string message, shared_ptr<Menu> last_menu, fs::path image_path)
      : ConfirmationMenu(message, last_menu), image_path(image_path) {};
    void onConfirmed();
};

#define ACTION_ADB_DEBUGGABLE 1001
#define ACTION_DELETE_DATA    1002
#define ACTION_FORMAT_DATA    1003
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

class DeleteDataConfirmationMenu : public ConfirmationMenu {
private:
    fs::path image_path;
public:
    DeleteDataConfirmationMenu(shared_ptr<Menu> last_menu, fs::path image_path)
      : ConfirmationMenu(
          "This will delete the userdata image file of the selected target.\n"
          "At the next boot, you will be prompted to create a new empty image.\n"
          "Continue?"
          , last_menu),
        image_path(image_path) {};
    void onConfirmed();
};

class FormatDataConfirmationMenu : public ConfirmationMenu {
private:
    fs::path target_image_path;
public:
    FormatDataConfirmationMenu(shared_ptr<Menu> last_menu, fs::path target_image_path)
      : ConfirmationMenu(
          "About to format the userdata image file of the selected target.\n"
          "This preserves the size of the image.\n"
          "Continue?"
        , last_menu),
        target_image_path(target_image_path) {};
    void onConfirmed();
};
