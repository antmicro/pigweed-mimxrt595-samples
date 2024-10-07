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

#include "bluetooth/addr.h"
#include "bluetooth/bluetooth.h"
#include "pw_async_basic/dispatcher.h"
#include "pw_bluetooth_sapphire/internal/host/common/random.h"
#include "pw_bluetooth_sapphire/internal/host/gap/adapter.h"
#include "pw_bluetooth_sapphire/internal/host/gap/bredr_connection_manager.h"
#include "pw_bluetooth_sapphire/internal/host/gap/low_energy_advertising_manager.h"
#include "pw_bluetooth_sapphire/internal/host/gap/peer_cache.h"
#include "pw_bluetooth_sapphire/internal/host/gatt/gatt.h"
#include "pw_bluetooth_sapphire/internal/host/l2cap/channel_manager.h"
#include "pw_bluetooth_sapphire/internal/host/transport/transport.h"
#include "pw_bluetooth_sapphire/public/pw_bluetooth_sapphire/internal/host/gap/pairing_delegate.h"
#include "pw_bluetooth_sapphire_mcuxpresso/controller.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

namespace bt::gap {
using namespace bt;
class MockPairingDelegate final : public PairingDelegate {
public:
  MockPairingDelegate() = default;

  // Returns the I/O capability of this delegate.
  sm::IOCapability io_capability() const override { return sm::IOCapability::kDisplayYesNo; }

  // Terminate any ongoing pairing challenge for the peer device with the given
  // |identifier|.
  void CompletePairing(PeerId peer_id, sm::Result<> status) override {
    (void)peer_id;
    (void)status;
    PW_LOG_DEBUG("UNIMPLEMENTED: CompletePairing");
  }

  // Ask the user to confirm the pairing request from the device with the given
  // |id| and confirm or reject by calling |confirm|.
  void ConfirmPairing(PeerId peer_id, ConfirmCallback confirm) override {
    (void)peer_id;
    confirm(true);
    PW_LOG_DEBUG("STUB: ConfirmPairing");
  }

  // Show the user the 6-digit |passkey| that should be compared to the peer's
  // passkey or entered into the peer. |confirm| may be called to accept a
  // comparison or to reject the pairing.
  void DisplayPasskey(PeerId peer_id,
                      uint32_t passkey,
                      DisplayMethod method,
                      ConfirmCallback confirm) override {
    (void)peer_id;
    passkey_ = passkey;
    (void)method;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    PW_LOG_DEBUG("STUB: DisplayPasskey, passkey: %ld", passkey);
    confirm(true);
  }

  // Ask the user to enter a 6-digit passkey or reject pairing. Report the
  // result by invoking |respond|.
  //
  // A valid |passkey| must be a non-negative integer. Pass a negative value to
  // reject pairing.
  using PasskeyResponseCallback = fit::callback<void(int64_t passkey)>;
  void RequestPasskey(PeerId peer_id,
                      PasskeyResponseCallback respond) override {
    (void)peer_id;
    respond(passkey_);
    PW_LOG_DEBUG("STUB: RequestPasskey");
  }
  uint32_t passkey_;
};
}  // namespace bt::gap

namespace pw::system {
using namespace bt;
using namespace bt::hci;
using namespace bt::gap;
using namespace bt::gatt;
using namespace bt::sdp;

void bluetooth_thread() {
  const DeviceAddress kLocalDevAddr(DeviceAddress::Type::kBREDR,
                                    {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC});
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

  adapter->SetLocalName("Pigweed A2DP Sink",
                        [](const auto& status) {
                          PW_LOG_INFO("Set local name: %d", status.is_ok());
                        });

  ServiceRecord record;
  record.SetServiceClassUUIDs({profile::kAudioSink});
  record.AddProtocolDescriptor(ServiceRecord::kPrimaryProtocolList,
                               protocol::kL2CAP,
                               DataElement(l2cap::kAVDTP));
  record.AddProtocolDescriptor(ServiceRecord::kPrimaryProtocolList,
                               protocol::kAVDTP,
                               DataElement(uint16_t{0x0103}));  // Version
  record.AddProfile(profile::kAdvancedAudioDistribution, 1, 3);
  record.SetAttribute(kA2DP_SupportedFeatures,
                      DataElement(uint16_t{0x0001}));  // Headphones
  std::vector<ServiceRecord> records;
  records.emplace_back(std::move(record));
  constexpr l2cap::ChannelParameters kChannelParams;

  MockPairingDelegate pairing_delegate;
  adapter->SetPairingDelegate(pairing_delegate.GetWeakPtr());

  adapter->bredr()->SetConnectable(
      true, [](const auto& status) {
        PW_LOG_INFO("Set connectable: %d", status.is_ok());
      });
  std::unique_ptr<bt::gap::BrEdrDiscoverableSession> session;
  adapter->bredr()->RequestDiscoverable(
      [&session](const auto& status, auto cb_session) {
        PW_LOG_INFO("Discoverable: %d", status.is_ok());
        session = std::move(cb_session);
      });
 auto handle = adapter->bredr()->RegisterService(
     records, kChannelParams, [](auto channel, const auto& data_element) {
       (void)channel;
       (void)data_element;
       PW_LOG_INFO("Service registered");
     });
 (void)handle;

  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// This will run once after pw::system::Init() completes. This callback must
// return or it will block the work queue.
void UserAppInit() {
  // Start new thread as WorkQueue thread stack is limited to 512.
  pw::thread::DetachedThread(pw::thread::freertos::Options(), bluetooth_thread);
}

}  // namespace pw::system
