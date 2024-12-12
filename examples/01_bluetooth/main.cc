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

#include <pw_random/xor_shift.h>

#include <memory>
#include <optional>

#include "pw_async_basic/dispatcher.h"
#include "pw_bluetooth_sapphire/internal/host/common/random.h"
#include "pw_bluetooth_sapphire/internal/host/gap/adapter.h"
#include "pw_bluetooth_sapphire/internal/host/gatt/gatt.h"
#include "pw_bluetooth_sapphire_mcuxpresso/controller.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

namespace pw::system {
using namespace bt::hci;
using namespace bt::gap;
using namespace bt::gatt;

void bluetooth_thread() {
  PW_LOG_INFO("Hello from bluetooth thread!");
  std::unique_ptr<pw::bluetooth::Controller> controller =
      std::make_unique<pw::bluetooth::Mimxrt595Controller>();
  pw::async::BasicDispatcher dispatcher;

  auto transport =
      std::make_unique<Transport>(std::move(controller), dispatcher);
  auto gatt = GATT::Create();

  pw::random::XorShiftStarRng64 rng(0);
  bt::set_random_generator(&rng);
  Adapter::Config config = {
      .legacy_pairing_enabled = false,
  };
  auto adapter = Adapter::Create(
      dispatcher, transport->GetWeakPtr(), gatt->GetWeakPtr(), config, nullptr);
  pw::sync::ThreadNotification transport_init_notification;
  adapter->Initialize(
      [&transport_init_notification](bool success) {
        transport_init_notification.release();
        PW_ASSERT(success);
        PW_LOG_INFO("Adapter initialized");
      },
      [] { PW_LOG_ERROR("Failed to initialize adapter"); });
  transport_init_notification.acquire();

  bt::AdvertisingData data;
  if (!data.SetLocalName("Pigweed Beacon")) {
    PW_LOG_CRITICAL("Failed to set local name");
  }
  std::vector<uint8_t> adv_data = {0x25, 0x00, 0xBC, 0x01, 0x02, 0x03, 0x04};
  if (!data.SetManufacturerData(0x25, bt::BufferView(adv_data))) {
    PW_LOG_CRITICAL("Failed to set manufacturer data");
  }

  adapter->SetLocalName("Pigweed A2DP Sink", [](const auto& status) {
    PW_LOG_INFO("Set local name: %d", status.is_ok());
  });

  adapter->le()->StartAdvertising(std::move(data),
                                  bt::AdvertisingData{},
                                  bt::gap::AdvertisingInterval::SLOW,
                                  0,
                                  0,
                                  0,
                                  std::nullopt,
                                  std::nullopt,
                                  [](auto instance, auto status) {
                                    (void)instance;
                                    PW_LOG_INFO("Advertising started: %d",
                                                status.is_ok());
                                    while (true) {
                                      vTaskDelay(1000);
                                    }
                                  });

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
