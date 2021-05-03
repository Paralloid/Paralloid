#include "ui.h"
#include <linux/input.h>
#include <sstream>
#include <string>

using namespace std;

namespace UI {
    shared_ptr<Menu> menu;
    string menu_title;
    shared_ptr<vector<string>> menu_items;
    int selected_item = 0;
    GRFont *font = nullptr;
    int font_width = 0, font_height = 0;
    int screen_width = 0;
    int characters_per_line = 0;
    
    void render();
    // All render functions below return the y position after
    // rendering their respective content.
    int renderTitle(int cur_y);
    int renderDivider(int cur_y);
    int renderMenu(int cur_y);
    // Text rendering routines with multi-line support
    int renderText(int cur_y, string text);
    // Breaks up long lines by default
    int renderLine(int cur_y, string line);
    
    int measureTextHeight(string text);
    
    int onInputEvent(int fd, uint32_t epevents);
}

using namespace UI;

#define SIDE_PADDING UI::font_width
#define DIVIDER_MARGIN (UI::font_height / 2)
#define MENU_ITEM_PADDING (UI::font_height / 2)

inline void setForegroundColor() {
    gr_color(255, 255, 255, 255);
}

inline void setForegroundColorInverse() {
    gr_color(0, 0, 0, 255);
}

int UI::init() {
   int res = gr_init();
   if (res < 0)
       return res;
   
   screen_width = gr_fb_width();
   
   // minui loads res/images/font.png by default
   font = (GRFont*) gr_sys_font();
   gr_font_size(font, &font_width, &font_height);
   characters_per_line = (screen_width - 2 * SIDE_PADDING) / font_width;
   
   if (characters_per_line >= 32) {
       // Use larger fonts on high-resolution screens
       gr_init_font("font_18x32", &font);
       gr_font_size(font, &font_width, &font_height);
       characters_per_line = (screen_width - 2 * SIDE_PADDING) / font_width;
   }
   
   return 0;
}

void UI::exit() {
    gr_exit();
}

void UI::switchMenu(shared_ptr<Menu> menu) {
    UI::menu = menu;
    menu_title = menu->getTitle();
    menu_items = menu->getItems();
    selected_item = 0;
    render();
}

void UI::render() {
    int cur_y = 0;
    
    // Reset the fb
    gr_color(0, 0, 0, 0);
    gr_clear();
    
    if (menu == nullptr) {
        return;
    }
    
    cur_y = renderTitle(cur_y);
    cur_y = renderMenu(cur_y);
    
    // Display the buffer on screen
    gr_flip();
}

int UI::renderText(int cur_y, string text) {
    istringstream s(text);
    string token;
    while (getline(s, token, '\n')) {
        cur_y = renderLine(cur_y, token);
    }
    return cur_y;
}

int UI::renderLine(int cur_y, string line) {
    if (line.length() <= characters_per_line) {
        gr_text(font, SIDE_PADDING, cur_y, line.c_str(), false);
        return cur_y + font_height;
    } else {
        for (int idx = 0; idx < line.length(); idx += characters_per_line) {
            cur_y = renderLine(cur_y, line.substr(idx, characters_per_line));
        }
        return cur_y;
    }
}

int UI::renderTitle(int cur_y) {
    setForegroundColor();
    return renderText(cur_y, menu_title);
}

int UI::measureTextHeight(string text) {
    int ret = 0;
    istringstream s(text);
    string token;
    while (getline(s, token, '\n')) {
        auto len = token.length();
        auto lines = len / characters_per_line + (len % characters_per_line != 0);
        ret += lines * font_height;
    }
    return ret;
}

int UI::renderDivider(int cur_y) {
    // A bit of margin between content and divider
    cur_y += DIVIDER_MARGIN;
    gr_fill(0, cur_y, screen_width, cur_y + 1);
    return cur_y + DIVIDER_MARGIN;
}

int UI::renderMenu(int cur_y) {
    // TODO: implement auto-paging on overflow
    setForegroundColor();
    
    // Divider above the menu items
    cur_y = renderDivider(cur_y);
    
    // Render menu items
    int idx = 0;
    for (auto& item : *menu_items) {
        if (idx == selected_item) {
            auto item_height = 2 * MENU_ITEM_PADDING + measureTextHeight(item);
            gr_fill(0, cur_y, screen_width, cur_y + item_height);
            setForegroundColorInverse();
        } else {
            setForegroundColor();
        }
        
        cur_y += MENU_ITEM_PADDING;
        cur_y = renderText(cur_y, item);
        cur_y += MENU_ITEM_PADDING;
        
        idx++;
    }
    
    setForegroundColor();
    
    // Divider below the menu items
    cur_y = renderDivider(cur_y);
    
    return cur_y;
}


void UI::runForever() {
    ev_init(&onInputEvent, false);
    while (true) {
        if (!ev_wait(500)) {
            ev_dispatch();
        }
    }
}

int UI::onInputEvent(int fd, uint32_t epevents) {
    struct input_event ev;
    if (ev_get_input(fd, epevents, &ev) == -1) {
        return -1;
    }
    
    if (ev.type != EV_KEY) {
        return -1;
    }
    
    if (ev.value != 0) {
        // We only handle key up events (value == 0)
        return -1;
    }
    
    if (ev.code == KEY_VOLUMEUP || ev.code == KEY_UP) {
        if (selected_item > 0) {
            selected_item--;
            render();
        }
    } else if (ev.code == KEY_VOLUMEDOWN || ev.code == KEY_DOWN) {
        if (selected_item < menu_items->size() - 1) {
            selected_item++;
            render();
        }
    }
    
    return 0;
}
