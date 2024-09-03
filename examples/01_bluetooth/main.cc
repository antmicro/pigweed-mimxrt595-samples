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

void bluetooth_thread() {
  PW_LOG_INFO("Hello from bluetooth thread!");
  std::unique_ptr<pw::bluetooth::Controller> controller =
      std::make_unique<pw::bluetooth::Mimxrt595Controller>();
  pw::async::BasicDispatcher dispatcher;

  pw::sync::ThreadNotification transport_init_notification;
  auto transport =
      std::make_unique<bt::hci::Transport>(std::move(controller), dispatcher);
  transport->Initialize([&transport_init_notification](bool success) {
    transport_init_notification.release();
    (void)success;
  });
  transport_init_notification.acquire();

  bt::hci::LowEnergyAdvertiserMimxrt595 advertiser(transport->GetWeakPtr(), 1);
  bt::DeviceAddress address;

  bt::AdvertisingData data;
  if (!data.SetLocalName("Pigweed Beacon")) {
    PW_LOG_CRITICAL("Failed to set local name");
  }
  std::vector<uint8_t> adv_data = {0x25, 0x00, 0xBC, 0x01, 0x02, 0x03, 0x04,
                                   0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                   0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0xc8};
  if (!data.SetManufacturerData(0x25, bt::BufferView(adv_data))) {
    PW_LOG_CRITICAL("Failed to set manufacturer data");
  }

  bt::AdvertisingData scan_rsp;
  bt::hci::LowEnergyAdvertiser::AdvertisingOptions options(
      bt::hci::AdvertisingIntervalRange(BT_GAP_ADV_FAST_INT_MIN_2,
                                        BT_GAP_ADV_FAST_INT_MAX_2),
      BT_LE_ADV_OPT_USE_IDENTITY,
      false,
      false,
      false);
  advertiser.StartAdvertising(
      address, data, scan_rsp, options, nullptr, nullptr);

  char addr_s[BT_ADDR_LE_STR_LEN];
  bt_addr_le_t addr = {.type = 0, .a = {0, 0, 0, 0, 0, 0}};
  size_t count = 1;
  bt_id_get(&addr, &count);
  bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

  PW_LOG_INFO("Beacon started, advertising as %s\n", addr_s);

  while (true) {
    vTaskDelay(1000);
  }
}

// This will run once after pw::system::Init() completes. This callback must
// return or it will block the work queue.
void UserAppInit() {
  // Start new thread as WorkQueue thread stack is limited to 512.
  pw::thread::DetachedThread(pw::thread::freertos::Options(), bluetooth_thread);
}

}  // namespace pw::system
