#pragma once
#include <memory>
#include <vector>
#include <string>

namespace UI {
    using namespace std;
    
    struct MenuItem {
        // A unique action ID associated with the item
        int action;
        // The displayed title of the item
        string title;
        
        MenuItem(int action, string title) : action(action), title(title) {}
    };
    
    class Menu {
    public:
        // Title and items must not change between when the menu is first
        // rendered and when the menu is replaced with another instance
        // These functions will only be called once when switchMenu() is
        // invoked and the result will be cached until the menu is replaced.
        virtual string getTitle() = 0;
        virtual shared_ptr<vector<MenuItem>> getItems() = 0;
        // The action ID is supplied by the `action` field in MenuItem
        virtual void onItemSelected(int action) = 0;
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
