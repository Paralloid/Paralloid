#pragma once
#include "ui.h"
#include <string>

using namespace std;

extern shared_ptr<UI::Menu> main_menu;

// `target` can be "internal", which means the system_phh internal partition,
// or path to the system image file. See the `boot-target` script for details
void boot_target(string target);
