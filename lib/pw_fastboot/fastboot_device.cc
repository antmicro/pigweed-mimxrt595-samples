/*
 * Copyright (C) 2024 Antmicro
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

#include "pw_fastboot/fastboot_device.h"

#include <algorithm>
#include <cstring>
#include <memory>

#include "pw_fastboot/constants.h"
#include "pw_fastboot/device_hal.h"
#include "pw_fastboot/device_variable.h"
#include "pw_log/log.h"
#include "stringutils/strings.h"

using std::string_literals::operator""s;

namespace pw::fastboot {

FastbootDevice::FastbootDevice(
    std::unique_ptr<Transport> transport,
    std::unique_ptr<DeviceVariableProvider> variables,
    std::unique_ptr<DeviceHAL> device_hal)
    : kCommandMap({
          {FB_CMD_DOWNLOAD, internal::DownloadHandler},
          {FB_CMD_GETVAR, internal::GetVarHandler},
          {FB_CMD_SHUTDOWN, internal::ShutDownHandler},
          {FB_CMD_REBOOT, internal::RebootHandler},
          {FB_CMD_REBOOT_BOOTLOADER, internal::RebootBootloaderHandler},
          {FB_CMD_REBOOT_FASTBOOT, internal::RebootFastbootHandler},
          {FB_CMD_REBOOT_RECOVERY, internal::RebootRecoveryHandler},
          {FB_CMD_FLASH, internal::FlashHandler},
          {FB_CMD_OEM, internal::OemCmdHandler},
      }),
      transport_(std::move(transport)),
      variables_(std::move(variables)),
      device_hal_(std::move(device_hal)) {}

FastbootDevice::~FastbootDevice() { CloseDevice(); }

void FastbootDevice::CloseDevice() { transport_->Close(); }

bool FastbootDevice::WriteStatus(FastbootResult result,
                                 const std::string& message) {
  // "FAIL", "OKAY", "INFO", "DATA"
  constexpr size_t kNumResponseTypes = 4;
  // The response reason occupies 4 bytes at the start of the message.
  // Larger response strings would overflow the message contents.
  constexpr size_t kResponseReasonSize = 4;
  constexpr const char* kResultStrings[kNumResponseTypes] = {
      RESPONSE_OKAY, RESPONSE_FAIL, RESPONSE_INFO, RESPONSE_DATA};
  // Sanity check these have the correct sizes; strlen is not constexpr,
  // so +1 for null terminator.
  static_assert(sizeof(RESPONSE_OKAY) == kResponseReasonSize + 1,
                "response string must be exactly 4 bytes");
  static_assert(sizeof(RESPONSE_FAIL) == kResponseReasonSize + 1,
                "response string must be exactly 4 bytes");
  static_assert(sizeof(RESPONSE_INFO) == kResponseReasonSize + 1,
                "response string must be exactly 4 bytes");
  static_assert(sizeof(RESPONSE_DATA) == kResponseReasonSize + 1,
                "response string must be exactly 4 bytes");

  char buf[FB_RESPONSE_SZ];
  constexpr size_t kMaxMessageSize = sizeof(buf) - kResponseReasonSize;
  size_t msg_len = std::min(kMaxMessageSize, message.size());

  if (static_cast<size_t>(result) >= kNumResponseTypes) {
    return false;
  }

  memcpy(buf, kResultStrings[static_cast<size_t>(result)], kResponseReasonSize);
  memcpy(buf + kResponseReasonSize, message.c_str(), msg_len);

  size_t response_len = kResponseReasonSize + msg_len;
  auto write_ret = this->get_transport()->Write(buf, response_len);
  if (write_ret != static_cast<ssize_t>(response_len)) {
    PW_LOG_ERROR("Failed to write %s", message.c_str());
    return false;
  }

  return true;
}

bool FastbootDevice::HandleData(bool read, std::vector<char>* data) {
  return HandleData(read, data->data(), data->size());
}

bool FastbootDevice::HandleData(bool read, char* data, uint64_t size) {
  auto read_write_data_size = read ? this->get_transport()->Read(data, size)
                                   : this->get_transport()->Write(data, size);
  if (read_write_data_size == -1) {
    PW_LOG_ERROR("%s transport failed", (read ? "read from" : "write to"));
    return false;
  }
  if (static_cast<size_t>(read_write_data_size) != size) {
    PW_LOG_ERROR("%s expected %lu bytes, got %u",
                 (read ? "read" : "write"),
                 (long unsigned int)size,
                 read_write_data_size);
    return false;
  }
  return true;
}

void FastbootDevice::ExecuteCommands() {
  char command[FB_RESPONSE_SZ + 1];
  for (;;) {
    auto bytes_read = transport_->Read(command, FB_RESPONSE_SZ);
    if (bytes_read == -1) {
      PW_LOG_ERROR("Couldn't read command");
      return;
    }
    if (std::count_if(command, command + bytes_read, iscntrl) != 0) {
      WriteStatus(FastbootResult::FAIL, "Command contains control character");
      continue;
    }
    command[bytes_read] = '\0';

    PW_LOG_INFO("Fastboot command: %s", command);

    std::vector<std::string> args;
    std::string cmd_name;
    if (cmd_name.starts_with("oem ")) {
      args = {command};
      cmd_name = FB_CMD_OEM;
    } else {
      args = stringutils::Split(command, ":");
      cmd_name = args[0];
    }

    auto found_command = kCommandMap.find(cmd_name);
    if (found_command == kCommandMap.end()) {
      WriteStatus(FastbootResult::FAIL, "Unrecognized command " + args[0]);
      continue;
    }
    if (!found_command->second(this, args)) {
      return;
    }
  }
}

bool FastbootDevice::WriteOkay(const std::string& message) {
  return WriteStatus(FastbootResult::OKAY, message);
}

bool FastbootDevice::WriteFail(const std::string& message) {
  return WriteStatus(FastbootResult::FAIL, message);
}

bool FastbootDevice::WriteInfo(const std::string& message) {
  return WriteStatus(FastbootResult::INFO, message);
}

}  // namespace pw::fastboot