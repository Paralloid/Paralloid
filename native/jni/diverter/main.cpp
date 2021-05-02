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
    gr_flip();
    
    while (true) {
        sleep(1000);
    }
    
    gr_exit();
    return 0;
}
