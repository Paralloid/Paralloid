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
#pragma once

#include <functional>
#include <string>
#include <vector>

constexpr unsigned int kMaxDownloadSizeDefault = 0x10000000;

enum class FastbootResult {
    OKAY,
    FAIL,
    INFO,
    DATA,
};

class FastbootDevice;

using CommandHandler = std::function<bool(FastbootDevice*, const std::vector<std::string>&)>; 

bool OemCmdHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool RebootHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool RebootRecoveryHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool RebootBootloaderHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool GetVarHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool DownloadHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool FlashHandler(FastbootDevice* device, const std::vector<std::string>& args);
