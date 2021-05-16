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
#include "constants.h"
#include "fastboot_device.h"
#include "flashing.h"

#include <iostream>

#include <android-base/parseint.h>
#include <android-base/stringprintf.h>

#include <private/images.h>
#include <private/utils.h>

void ListImages(FastbootDevice *device, std::string base_path) {
    auto images = BootableImage::scanImages(base_path);
    
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
        device->WriteInfo("Hello! I am minfastbootd from Paralloid");
        return device->WriteStatus(FastbootResult::OKAY, "");
    } else if (args[0] == "oem list-images") {
        return ListImagesHandler(device, args);
    } else {
        return device->WriteStatus(FastbootResult::FAIL, "Unsupported OEM command");
    }
}

bool RebootHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    device->WriteStatus(FastbootResult::OKAY, "Rebooting");
    rebootInto(nullptr);
    return true;
}

bool RebootRecoveryHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    device->WriteStatus(FastbootResult::OKAY, "Rebooting into recovery");
    rebootInto("recovery");
    return true;
}

bool RebootBootloaderHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    device->WriteStatus(FastbootResult::OKAY, "Rebooting into bootloader");
    rebootInto("bootloader");
    return true;
}

bool GetVarHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    if (args[1] == FB_VAR_MAX_DOWNLOAD_SIZE) {
        // This is needed for max download size to be recognized correctly
        return device->WriteOkay(android::base::StringPrintf("0x%X", kMaxDownloadSizeDefault));
    } else {
        return device->WriteStatus(FastbootResult::FAIL, "Unknown variable");
    }
}

bool DownloadHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return device->WriteStatus(FastbootResult::FAIL, "size argument unspecified");
    }
    
    // arg[0] is the command name, arg[1] contains size of data to be downloaded
    unsigned int size;
    if (!android::base::ParseUint("0x" + args[1], &size, kMaxDownloadSizeDefault)) {
        return device->WriteStatus(FastbootResult::FAIL, "Invalid size");
    }
    
    device->download_data().resize(size);
    
    if (!device->WriteStatus(FastbootResult::DATA, android::base::StringPrintf("%08x", size))) {
        return false;
    }
    
    if (device->HandleData(true, &device->download_data())) {
        return device->WriteStatus(FastbootResult::OKAY, "");
    }
    
    std::cout << "Couldn't download data" << std::endl;
    return device->WriteStatus(FastbootResult::FAIL, "Couldn't download data");
}

bool FlashHandler(FastbootDevice* device, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return device->WriteStatus(FastbootResult::FAIL, "Invalid arguments");
    }
    
    const auto& target = args[1];
    int ret = Flash(device, target);
    
    if (ret < 0) {
        return device->WriteStatus(FastbootResult::FAIL, strerror(-ret));
    }
    
    return device->WriteStatus(FastbootResult::OKAY, "Flashing succeeded");
}
