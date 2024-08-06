// Copyright 2023 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#define PW_LOG_MODULE_NAME "MAIN"

#include <memory>

#include "bluetooth/addr.h"
#include "bluetooth/bluetooth.h"
#include "pw_async_basic/dispatcher.h"
#include "pw_bluetooth_sapphire/internal/host/gap/low_energy_advertising_manager.h"
#include "pw_bluetooth_sapphire/internal/host/transport/transport.h"
#include "pw_bluetooth_sapphire_mcuxpresso/controller.h"
#include "pw_bluetooth_sapphire_mcuxpresso/low_energy_advertiser.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

namespace pw::system {

void example_thread_function() { PW_LOG_INFO("Hello from example thread!"); }

// This will run once after pw::system::Init() completes. This callback must
// return or it will block the work queue.
void UserAppInit() {
  pw::thread::DetachedThread(pw::thread::freertos::Options(),
                             example_thread_function);

  std::unique_ptr<pw::bluetooth::Controller> controller =
      std::make_unique<pw::bluetooth::Mimxrt595Controller>();
  pw::async::BasicDispatcher dispatcher;
  auto transport =
      std::make_unique<bt::hci::Transport>(std::move(controller), dispatcher);
  transport->Initialize([](bool success) {});

  // TODO: it seems that there is a race-condition between controller
  // initialization callback and advertising start. Low Energy Advertiser
  // assumes, that transport is already initialized. Add small delay to
  // workaround this issue.
  vTaskDelay(1000);

  bt::hci::LowEnergyAdvertiserMimxrt595 advertiser(transport->GetWeakPtr(), 1);
  bt::DeviceAddress address(bt::DeviceAddress::Type::kLEPublic,
                            {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA});

  bt::AdvertisingData data;
  bt::AdvertisingData scan_rsp;
  bt::hci::LowEnergyAdvertiser::AdvertisingOptions options(
      bt::hci::AdvertisingIntervalRange(1000, 10000), 0, false, false, false);
  advertiser.StartAdvertising(
      address, data, scan_rsp, options, nullptr, nullptr);

  char addr_s[BT_ADDR_LE_STR_LEN];
  bt_addr_le_t addr = {0};
  size_t count = 1;
  bt_id_get(&addr, &count);
  bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

  PW_LOG_CRITICAL("Beacon started, advertising as %s\n", addr_s);

  PW_LOG_CRITICAL("Hello, world!");

  while (true) {
    vTaskDelay(1000);
  }
  return;
}

}  // namespace pw::system
