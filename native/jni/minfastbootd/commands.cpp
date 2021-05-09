/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "commands.h"
#include "fastboot_device.h"

#include <android-base/stringprintf.h>
#include <images.h>
#include <sys/reboot.h>

void ListImages(FastbootDevice *device, std::string base_path) {
    auto images = scanImages(base_path);
    
    for (auto& img : images) {
        device->WriteInfo(android::base::StringPrintf("    %s", img.imageName().c_str()));
        // For anything to be eligible here, system.img should at least exist
        device->WriteInfo("      - system");
        
        if (fs::exists(img.productImagePath())) {
            device->WriteInfo("      - product");
        }
    }
}

bool ListImagesHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    if (fs::exists(USERDATA_BASE_PATH)) {
        device->WriteInfo("Images in userdata:");
        ListImages(device, USERDATA_BASE_PATH);
    }
    
    if (fs::exists(EXT_SDCARD_BASE_PATH)) {
        device->WriteInfo("Images in sdcard:");
        ListImages(device, EXT_SDCARD_BASE_PATH);
    }
    
    return device->WriteStatus(FastbootResult::OKAY, "Images listed");
}

bool OemCmdHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    if (args[0] == "oem hello") {
        device->WriteInfo("Hello! I am minfastbootd from boot-diverter");
        return device->WriteStatus(FastbootResult::OKAY, "");
    } else if (args[0] == "oem list-images") {
        return ListImagesHandler(device, args);
    } else {
        return device->WriteStatus(FastbootResult::FAIL, "Unsupported OEM command");
    }
}

bool RebootHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    device->WriteStatus(FastbootResult::OKAY, "Rebooting");
    reboot(RB_AUTOBOOT);
    return true;
}
