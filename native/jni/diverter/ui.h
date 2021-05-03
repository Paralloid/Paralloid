#pragma once
#include <memory>
#include <vector>

namespace UI {
    using namespace std;
    
    class Menu {
    public:
        // Title and items must not change between when the menu is first
        // rendered and when the menu is replaced with another instance
        // These functions will only be called once when switchMenu() is
        // invoked and the result will be cached until the menu is replaced.
        virtual string getTitle() = 0;
        virtual shared_ptr<vector<string>> getItems() = 0;
        // The ID is the index of the current selected item in the list last
        // returned by getItems();
        virtual void onItemSelected(int id) = 0;
    };
    
    // Must be called before anything in the namespace is used
    int init();
    // Must be called before the program exits or exec() another program
    void exit();
    // The new menu is rendered immediately
    void switchMenu(shared_ptr<Menu> menu);
    // Run the event loop
    void runForever();
}
