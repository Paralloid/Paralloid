#include <iostream>
#include <minui/minui.h>
#include <unistd.h>

using namespace std;

int main() {
    if (gr_init() < 0) {
        cout << "minui init failed" << endl;
    }
    
    gr_color(0, 0, 0, 0);
    gr_clear();
    gr_color(255, 255, 255, 128);
    gr_fill(0, 0, 400, 400);
    // TODO: maybe we should change font size by detecting device DPI somehow
    // (e.g. using the estimated characters per line; if larger than a threshold
    //       then switch to the larger font)
    gr_text(gr_sys_font(), 0, 410, "Hello, world", false);
    gr_flip();
    
    while (true) {
        sleep(1000);
    }
    
    gr_exit();
    return 0;
}
