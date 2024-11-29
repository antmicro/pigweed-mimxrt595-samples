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

#include "pw_async_basic/dispatcher.h"
#include "pw_bluetooth_sapphire/internal/host/common/random.h"
#include "pw_bluetooth_sapphire/internal/host/gap/adapter.h"
#include "pw_bluetooth_sapphire/internal/host/gap/bredr_connection_manager.h"
#include "pw_bluetooth_sapphire/public/pw_bluetooth_sapphire/internal/host/gap/pairing_delegate.h"
#include "pw_bluetooth_sapphire_mcuxpresso/controller.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"
#include "sbc.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avdtp_internal.h"
#include "netbuf.h"

namespace bt::gap {
using namespace bt;
class MockPairingDelegate final : public PairingDelegate {
 public:
  MockPairingDelegate() = default;

  // Returns the I/O capability of this delegate.
  sm::IOCapability io_capability() const override {
    return sm::IOCapability::kDisplayYesNo;
  }

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

static struct bt_a2dp_stream sbc_stream;

void stream_configured(struct bt_a2dp_stream* stream) {
  (void)stream;
  PW_LOG_DEBUG("stream configured");
}

void stream_established(struct bt_a2dp_stream* stream) {
  (void)stream;
  PW_LOG_DEBUG("stream established");
}

void stream_released(struct bt_a2dp_stream* stream) {
  (void)stream;
  PW_LOG_DEBUG("stream released");
}

void stream_started(struct bt_a2dp_stream* stream) {
  (void)stream;
  PW_LOG_DEBUG("stream started");
}

void sink_sbc_streamer_data(struct bt_a2dp_stream* stream,
                            struct net_buf* buf,
                            uint16_t seq_num,
                            uint32_t ts) {
  (void)stream;
  (void)seq_num;
  (void)ts;
  uint8_t sbc_hdr;

  sbc_hdr = net_buf_pull_u8(buf);
  PW_LOG_DEBUG("received, num of frames: %d, data length:%d",
               (uint8_t)sbc_hdr & 0x7,
               buf->len());
  PW_LOG_DEBUG("data: %d, %d, %d, %d, %d, %d ......",
               buf->data[0],
               buf->data[1],
               buf->data[2],
               buf->data[3],
               buf->data[4],
               buf->data[5]);
}

void stream_recv(struct bt_a2dp_stream* stream,
                 struct net_buf* buf,
                 uint16_t seq_num,
                 uint32_t ts) {
  sink_sbc_streamer_data(stream, buf, seq_num, ts);
}

static struct bt_a2dp_stream_ops stream_ops = {
    .configured = stream_configured,
    .established = stream_established,
    .released = stream_released,
    .started = stream_started,
    .suspended = NULL,
    .reconfigured = NULL,
#if defined(CONFIG_BT_A2DP_SINK)
    .recv = stream_recv,
#endif
#if defined(CONFIG_BT_A2DP_SOURCE)
    .sent = NULL,
#endif
};

void app_connected(struct bt_a2dp* a2dp, int err) {
  (void)a2dp;
  if (!err) {
    // default_a2dp = a2dp;
    PW_LOG_DEBUG("a2dp connected");
  } else {
    PW_LOG_DEBUG("a2dp connecting fail");
  }
}

void app_disconnected(struct bt_a2dp* a2dp) {
  (void)a2dp;
  // found_peer_sbc_endpoint = NULL;
  PW_LOG_DEBUG("a2dp disconnected");
}

int app_config_req(/*struct bt_a2dp *a2dp,*/ struct bt_a2dp_ep* ep,
                   struct bt_a2dp_codec_cfg* codec_cfg,
                   struct bt_a2dp_stream** stream,
                   uint8_t* rsp_err_code) {
  (void)ep;
  bt_a2dp_stream_cb_register(&sbc_stream, &stream_ops);
  *stream = &sbc_stream;
  *rsp_err_code = 0;

  PW_LOG_DEBUG("receive requesting config and accept");
  if (*rsp_err_code == 0) {
    uint32_t sample_rate;

    PW_LOG_DEBUG("SBC configure success");
    sample_rate = bt_a2dp_sbc_get_sampling_frequency(
        (struct bt_a2dp_codec_sbc_params*)&codec_cfg->codec_config
            ->codec_ie[0]);
    PW_LOG_DEBUG("sample rate %ldHz", sample_rate);
  } else {
    PW_LOG_DEBUG("configure err");
  }
  return 0;
}

void app_config_rsp(struct bt_a2dp_stream* stream, uint8_t rsp_err_code) {
  (void)stream;
  if (rsp_err_code == 0) {
    PW_LOG_DEBUG("success to configure");
  } else {
    PW_LOG_DEBUG("fail to configure");
  }
}

int app_establish_req(struct bt_a2dp_stream* stream, uint8_t* rsp_err_code) {
  (void)stream;
  *rsp_err_code = 0;
  PW_LOG_DEBUG("receive requesting establishment and accept");
  return 0;
}

void app_establish_rsp(struct bt_a2dp_stream* stream, uint8_t rsp_err_code) {
  (void)stream;
  if (rsp_err_code == 0) {
    PW_LOG_DEBUG("success to establish");
  } else {
    PW_LOG_DEBUG("fail to establish");
  }
}

int app_start_req(struct bt_a2dp_stream* stream, uint8_t* rsp_err_code) {
  (void)stream;
  *rsp_err_code = 0;
  PW_LOG_DEBUG("receive requesting start and accept");
  return 0;
}

void app_start_rsp(struct bt_a2dp_stream* stream, uint8_t rsp_err_code) {
  (void)stream;
  if (rsp_err_code == 0) {
    PW_LOG_DEBUG("success to start");
  } else {
    PW_LOG_DEBUG("fail to start");
  }
}

struct bt_a2dp_cb a2dp_cb = {
    .connected = app_connected,
    .disconnected = app_disconnected,
    .config_req = app_config_req,
    .config_rsp = app_config_rsp,
    .establish_req = app_establish_req,
    .establish_rsp = app_establish_rsp,
    .release_req = NULL,
    .release_rsp = NULL,
    .start_req = app_start_req,
    .start_rsp = app_start_rsp,
    .suspend_req = NULL,
    .suspend_rsp = NULL,
    .reconfig_req = NULL,
    .reconfig_rsp = NULL,
};

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
  adapter->SetDeviceClass(
      DeviceClass(DeviceClass::MajorClass::kAudioVideo),
      [](auto status) { PW_LOG_INFO("Set device class: %d", status.is_ok()); });

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

  ServiceRecord record;
  record.SetServiceClassUUIDs({profile::kAudioSink});
  record.AddProtocolDescriptor(ServiceRecord::kPrimaryProtocolList,
                               protocol::kL2CAP,
                               DataElement(l2cap::kAVDTP));
  record.AddProtocolDescriptor(ServiceRecord::kPrimaryProtocolList,
                               protocol::kAVDTP,
                               DataElement(uint16_t{0x0103}));  // Version
  record.AddProfile(profile::kAdvancedAudioDistribution, 1, 3);
  std::vector<ServiceRecord> records;
  records.emplace_back(std::move(record));
  constexpr l2cap::ChannelParameters kChannelParams;

  MockPairingDelegate pairing_delegate;
  adapter->SetPairingDelegate(pairing_delegate.GetWeakPtr());

  adapter->bredr()->SetConnectable(true, [](const auto& status) {
    PW_LOG_INFO("Set connectable: %d", status.is_ok());
  });
  std::unique_ptr<bt::gap::BrEdrDiscoverableSession> session;
  adapter->bredr()->RequestDiscoverable(
      [&session](const auto& status, auto cb_session) {
        PW_LOG_INFO("Discoverable: %d", status.is_ok());
        session = std::move(cb_session);
      });

  std::vector<l2cap::Channel::WeakPtr> channels;
  BT_A2DP_SBC_SINK_EP_DEFAULT(sink_sbc_endpoint);
  PW_LOG_INFO("SBC codec length: %d", sink_sbc_endpoint.codec_cap->len);
  for (int i = 0; i < sink_sbc_endpoint.codec_cap->len; i++) {
    PW_LOG_INFO("SBC codec capability: 0x%x",
                sink_sbc_endpoint.codec_cap->codec_ie[i]);
  }

  // bt_avdtp_register_sep(BT_AVDTP_AUDIO, BT_AVDTP_SINK,
  // &sink_sbc_endpoint.sep);
  bt_a2dp_register_cb(&a2dp_cb);
  PW_LOG_INFO(
      "bt_a2dp_register_ep: %d",
      bt_a2dp_register_ep(&sink_sbc_endpoint, BT_AVDTP_AUDIO, BT_AVDTP_SINK));
  bt_a2dp_init_zephyr();
  auto handle = adapter->bredr()->RegisterService(
      records,
      kChannelParams,
      [&channels](auto channel, const auto& data_element) {
        (void)data_element;
        //struct bt_avdtp bt_avdtp_channel = {
        //    .br_chan = {.chan = {.chan = channel}},
        //    .req = nullptr,
        //    .ops = nullptr,
        //    .current_sep = nullptr,
        //};
        struct bt_conn* conn = nullptr;
        struct bt_avdtp* avdtp_session;
        a2dp_accept(conn, &avdtp_session);
        avdtp_session->br_chan.chan.chan = channel;
        PW_LOG_INFO("Service registered");
        channel->Activate(
            [channel, avdtp_session](auto buffer) {
              avdtp_session->br_chan.chan.chan = channel;
              struct net_buf buf;
              for (auto& b : *buffer) {
                // PW_LOG_INFO("Received data: 0x%x", b);
                buf.data.push_back(b);
              }
              if ((buf.data[1] & 0x3f) == 0x20) {
                // audio stream data
                return;
              }

              bt_avdtp_l2cap_recv(&avdtp_session->br_chan.chan, &buf);
            },
            []() { PW_LOG_INFO("Channel closed"); });
        channels.emplace_back(std::move(channel));
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
