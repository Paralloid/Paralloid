#pragma once
#include "ui.h"

using namespace std;
using namespace UI;

class CachedMenu : public Menu {
protected:
    shared_ptr<vector<MenuItem>> items;
    virtual void populateItems() = 0;
public:
    shared_ptr<vector<MenuItem>> getItems();
};

#define ACTION_BOOT_INTERNAL 1001
#define ACTION_BOOT_SDCARD   1002
class MainMenu : public CachedMenu {
public:
    string getTitle();
    void populateItems();
    void onItemSelected(int action);
};
