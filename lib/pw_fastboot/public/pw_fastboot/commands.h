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

namespace pw::fastboot {

class FastbootDevice;

enum class FastbootResult {
  OKAY,
  FAIL,
  INFO,
  DATA,
};

namespace internal {

constexpr unsigned int kMaxDownloadSizeDefault = 0x10000000;
constexpr unsigned int kMaxFetchSizeDefault = 0x10000000;

// Execute a command with the given arguments (possibly empty).
using CommandHandler =
    std::function<bool(FastbootDevice*, const std::vector<std::string>&)>;

bool DownloadHandler(FastbootDevice* device,
                     const std::vector<std::string>& args);
bool ShutDownHandler(FastbootDevice* device,
                     const std::vector<std::string>& args);
bool RebootHandler(FastbootDevice* device,
                   const std::vector<std::string>& args);
bool RebootBootloaderHandler(FastbootDevice* device,
                             const std::vector<std::string>& args);
bool RebootFastbootHandler(FastbootDevice* device,
                           const std::vector<std::string>& args);
bool RebootRecoveryHandler(FastbootDevice* device,
                           const std::vector<std::string>& args);
bool GetVarHandler(FastbootDevice* device,
                   const std::vector<std::string>& args);
bool FlashHandler(FastbootDevice* device, const std::vector<std::string>& args);
bool OemCmdHandler(FastbootDevice* device,
                   const std::vector<std::string>& args);

}  // namespace internal
}  // namespace pw::fastboot