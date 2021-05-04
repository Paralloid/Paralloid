#include "ui.h"
#include <linux/input.h>
#include <minui/minui.h>
#include <sstream>
#include <string>

using namespace std;

namespace UI {
    shared_ptr<Menu> menu;
    string menu_title;
    shared_ptr<vector<string>> menu_items;
    // Item indices start from 0 and end at menu_items->size() - 1
    int selected_item = 0;
    int first_rendered_item = 0;
    
    GRFont *font = nullptr;
    int font_width = 0, font_height = 0;
    int screen_width = 0, screen_height = 0;
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
    int measureItemHeight(int idx);
    
    // Find the last item that can be displayed within remaining_height
    // assuming `first` is the first item that's going to be rendered
    int findLastDisplayableItem(int first, int remaining_height);
    // Find the first item that can be displayed within remaining_height
    // assuming `last` is the last item that's going to be rendered
    int findFirstDisplayableItem(int last, int remaining_height);
    
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
   screen_height = gr_fb_height();
   
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

int UI::measureItemHeight(int idx) {
    return 2 * MENU_ITEM_PADDING + measureTextHeight(menu_items->at(idx));
}

int UI::renderDivider(int cur_y) {
    // A bit of margin between content and divider
    cur_y += DIVIDER_MARGIN;
    gr_fill(0, cur_y, screen_width, cur_y + 1);
    return cur_y + DIVIDER_MARGIN;
}

int UI::findLastDisplayableItem(int first, int remaining_height) {
    int ret = first;
    int acc_height = 0;
    for (; ret < menu_items->size(); ret++) {
        auto item_height = measureItemHeight(ret);
        if (acc_height + item_height > remaining_height) {
            // We may have overshot
            // The case ret == first will be handled in the bounds checks below
            ret--;
            break;
        }
        acc_height += item_height;
    }
    
    if (ret < first) {
        ret = first;
    }
    
    if (ret >= menu_items->size()) {
        ret = menu_items->size() - 1;
    }
    
    return ret;
}

int UI::findFirstDisplayableItem(int last, int remaining_height) {
    int ret = last;
    int acc_height = 0;
    
    for (ret = last; ret >= 0; ret--) {
        auto item_height = measureItemHeight(ret);
        if (acc_height + item_height > remaining_height) {
            ret++;
            break;
        }
        acc_height += item_height;
    }
    
    if (ret > last) {
        ret = last;
    }
        
    if (ret < 0) {
        ret = 0;
    }
    
    return ret;
}

int UI::renderMenu(int cur_y) {
    setForegroundColor();
    
    // Divider above the menu items
    cur_y = renderDivider(cur_y);
    
    // Make sure selected_item is always in view
    if (first_rendered_item > selected_item) {
        first_rendered_item = selected_item;
    }
    
    int remaining_height = screen_height - cur_y - 2 * DIVIDER_MARGIN - 1 - font_height;
    int last_item = findLastDisplayableItem(first_rendered_item, remaining_height);
    
    if (last_item < selected_item) {
        // We cannot show selected_item if we keep first_rendered_item unchanged
        // So we calculate first_rendered_item backwards from selected_item assuming
        // selected_item is placed at the very bottom of the screen
        first_rendered_item = findFirstDisplayableItem(selected_item, remaining_height);
        // There may be space left for more items after selected_item; this is because
        // we *assumed* selected_item is the last item to display in the last step.
        // However, if items after selected_item are very short while those before are
        // really long, then we may have to display more of the latter instead of the former.
        last_item = findLastDisplayableItem(first_rendered_item, remaining_height);
    }
    
    // Render menu items
    for (int idx = first_rendered_item; idx <= last_item; idx++) {
        if (idx == selected_item) {
            auto item_height = measureItemHeight(idx);
            gr_fill(0, cur_y, screen_width, cur_y + item_height);
            setForegroundColorInverse();
        } else {
            setForegroundColor();
        }
        
        cur_y += MENU_ITEM_PADDING;
        cur_y = renderText(cur_y, menu_items->at(idx));
        cur_y += MENU_ITEM_PADDING;
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
        } else {
            selected_item = menu_items->size() - 1;
        }
        
        render();
    } else if (ev.code == KEY_VOLUMEDOWN || ev.code == KEY_DOWN) {
        if (selected_item < menu_items->size() - 1) {
            selected_item++;
        } else {
            selected_item = 0;
        }
        
        render();
    } else if (ev.code == KEY_POWER) {
        menu->onItemSelected(selected_item);
    }
    
    return 0;
}
