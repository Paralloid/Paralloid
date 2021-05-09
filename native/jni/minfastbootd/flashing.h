#pragma once
#include <string>
#include <vector>

class FastbootDevice;
int Flash(FastbootDevice* device, const std::string& target);
