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

#include "sbc.h"
#include "BT_jpl_api.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avdtp_internal.h"
#include "board.h"
#include "fsl_adapter_audio.h"
#include "fsl_codec_common.h"
#include "fsl_wm8904.h"
#include "netbuf.h"
#include "pw_async_basic/dispatcher.h"
#include "pw_bluetooth_sapphire/internal/host/common/random.h"
#include "pw_bluetooth_sapphire/internal/host/gap/pairing_delegate.h"
#include "pw_bluetooth_sapphire_mcuxpresso/controller.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"
#include "pw_random/xor_shift.h"
#include "sbc_api.h"

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

#define BOARD_CODEC_I2C_INSTANCE 0
#define DEMO_AUDIO_INSTANCE (3)
#define EXAMPLE_DMA_INSTANCE (0)
#define EXAMPLE_TX_CHANNEL (7)
#define DEMO_AUDIO_DATA_CHANNEL (kHAL_AudioStereo)
#define DEMO_AUDIO_BIT_WIDTH (kHAL_AudioWordWidth16bits)
#define DEMO_AUDIO_SAMPLING_RATE (kHAL_AudioSampleRate44100Hz)
#define A2DP_CODEC_DAC_VOLUME (100U) /* Range: 0 ~ 100 */
#define A2DP_CODEC_HP_VOLUME (100U)  /* Range: 0 ~ 100 */

static struct bt_a2dp_stream sbc_stream;

hal_audio_dma_config_t audioTxDmaConfig = {
    .instance = EXAMPLE_DMA_INSTANCE,
    .channel = EXAMPLE_TX_CHANNEL,
    .priority = kHAL_AudioDmaChannelPriorityDefault,
    .enablePreemption = false,
    .enablePreemptAbility = false,
    .dmaMuxConfig = NULL,
    .dmaChannelMuxConfig = NULL,
};

hal_audio_config_t audioTxConfig = {
    .dmaConfig = &audioTxDmaConfig,
    .ipConfig = NULL,
    .srcClock_Hz = 0,
    .sampleRate_Hz = (uint32_t)DEMO_AUDIO_SAMPLING_RATE,
    .fifoWatermark = 0,
    .masterSlave = kHAL_AudioSlave,
    .bclkPolarity = kHAL_AudioSampleOnRisingEdge,
    .frameSyncWidth = kHAL_AudioFrameSyncWidthHalfFrame,
    .frameSyncPolarity = kHAL_AudioBeginAtFallingEdge,
    .lineChannels = DEMO_AUDIO_DATA_CHANNEL,
    .dataFormat = kHAL_AudioDataFormatI2sClassic,
    .bitWidth = (uint8_t)DEMO_AUDIO_BIT_WIDTH,
    .instance = DEMO_AUDIO_INSTANCE,
};

wm8904_config_t wm8904Config = {
    .master = true,
    .protocol = kWM8904_ProtocolI2S,
    .format = {.sampleRate = kWM8904_SampleRate48kHz,
               .bitWidth = kWM8904_BitWidth16},
    .recordSource = kWM8904_RecordSourceLineInput,
    .recordChannelLeft = kWM8904_RecordChannelLeft2,
    .recordChannelRight = kWM8904_RecordChannelRight2,
    .playSource = kWM8904_PlaySourceDAC,
    .slaveAddress = WM8904_I2C_ADDRESS,
    .i2cConfig = {.codecI2CInstance = BOARD_CODEC_I2C_INSTANCE},
};

codec_config_t boardCodecConfig = {.codecDevType = kCODEC_WM8904,
                                   .codecDevConfig = &wm8904Config};

const clock_audio_pll_config_t audioPllConfig = {
    .audio_pll_src = kCLOCK_AudioPllXtalIn, /* OSC clock */
    .numerator =
        5040, /* Numerator of the Audio PLL fractional loop divider is null */
    .denominator = 13125, /* Denominator of the Audio PLL fractional loop
                             divider is null */
    .audio_pll_mult = kCLOCK_AudioPllMult20 /* Divide by 20 */
};

const clock_audio_pll_config_t audioPllConfig1 = {
    .audio_pll_src = kCLOCK_AudioPllXtalIn, /* OSC clock */
    .numerator =
        5040, /* Numerator of the Audio PLL fractional loop divider is null */
    .denominator = 27000, /* Denominator of the Audio PLL fractional loop
                             divider is null */
    .audio_pll_mult = kCLOCK_AudioPllMult22 /* Divide by 22 */
};

static HAL_AUDIO_HANDLE_DEFINE(audio_tx_handle);
static codec_handle_t codec_handle;

static pw::sync::ThreadNotification audio_cond;
static uint8_t a2dp_sbc_buffer[JPL_SBC_FRAME_SIZE * JPL_SBC_NUM_FRAMES];
static uint8_t a2dp_pcm_buffer[JPL_PCM_BLOCK_SIZE * JPL_PCM_NUM_BLOCKS];
static uint8_t a2dp_silence_buffer[JPL_PCM_BLOCK_SIZE];
static uint8_t* pcm_buffer_ptr = &a2dp_pcm_buffer[0];

static JPL_PARAM jpl_param = {
    .pcm_num_frames = ((3 * (SBC_MAX_BLOCK_SIZE * SBC_MAX_SUBBAND)) / (16 * 8)),
    .pod_frames = ((18 * (SBC_MAX_BLOCK_SIZE * SBC_MAX_SUBBAND)) / (16 * 8)),
    .sbc_param = {.sampling_frequency = 3,  // 48k
                  .nrof_blocks = 16,
                  .channel_mode = 3,  // joint stereo
                  .nrof_channels = 2,
                  .allocation_method = 0,  // loudness
                  .nrof_subbands = 8,      // subbands
                  .bitpool = 35}};

static void frame_remove_task() {
  while (true) {
    audio_cond.acquire();
    BT_jpl_remove_frames(pcm_buffer_ptr, JPL_PCM_BLOCK_SIZE);
    pcm_buffer_ptr += JPL_PCM_BLOCK_SIZE;
    if (pcm_buffer_ptr >=
        a2dp_pcm_buffer + sizeof(a2dp_pcm_buffer)) {
      pcm_buffer_ptr = a2dp_pcm_buffer;
    }
  }
}

static void tx_callback(hal_audio_handle_t handle,
                        hal_audio_status_t completionStatus,
                        void* callbackParam) {
  (void)handle;
  (void)callbackParam;
  (void)completionStatus;
  audio_cond.release();
}

uint32_t BOARD_SwitchAudioFreq(uint32_t sampleRate) {
  CLOCK_DeinitAudioPll();

  if (0U == sampleRate) {
    /* Disable MCLK output */
    SYSCTL1->MCLKPINDIR &= ~SYSCTL1_MCLKPINDIR_MCLKPINDIR_MASK;
  } else {
    if (44100U == sampleRate) {
      CLOCK_InitAudioPll(&audioPllConfig);
    } else if (0U == sampleRate % 8000U) {
      CLOCK_InitAudioPll(&audioPllConfig1);
    } else {
      /* no action */
    }

    CLOCK_InitAudioPfd(kCLOCK_Pfd0, 26); /* Enable Audio PLL clock */
    CLOCK_SetClkDiv(kCLOCK_DivAudioPllClk,
                    15U); /* Set AUDIOPLLCLKDIV divider to value 15 */

    /* Attach main clock to I3C, 396MHz / 16 = 24.75MHz. */
    CLOCK_AttachClk(kMAIN_CLK_to_I3C_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivI3cClk, 16);

    /* attach AUDIO PLL clock to FLEXCOMM1 (I2S1) */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM1);
    /* attach AUDIO PLL clock to FLEXCOMM3 (I2S3) */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM3);

    /* attach AUDIO PLL clock to MCLK (AudioPll * (18 / 26) / 15 / 1 = 24.576MHz
     * / 22.5792MHz) */
    CLOCK_AttachClk(kAUDIO_PLL_to_MCLK_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivMclkClk, 1);
    SYSCTL1->MCLKPINDIR = SYSCTL1_MCLKPINDIR_MCLKPINDIR_MASK;

    /* Set shared signal set 0: SCK, WS from Flexcomm1 */
    SYSCTL1->SHAREDCTRLSET[0] = SYSCTL1_SHAREDCTRLSET_SHAREDSCKSEL(1) |
                                SYSCTL1_SHAREDCTRLSET_SHAREDWSSEL(1);
    /* Set flexcomm3 SCK, WS from shared signal set 0 */
    SYSCTL1->FCCTRLSEL[3] =
        SYSCTL1_FCCTRLSEL_SCKINSEL(1) | SYSCTL1_FCCTRLSEL_WSINSEL(1);

    wm8904Config.i2cConfig.codecI2CSourceClock = CLOCK_GetI3cClkFreq();
    wm8904Config.mclk_HZ = CLOCK_GetMclkClkFreq();
    switch (sampleRate) {
      case 8000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate8kHz;
        break;
      case 11025:
        wm8904Config.format.sampleRate = kWM8904_SampleRate11025Hz;
        break;
      case 12000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate12kHz;
        break;
      case 16000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate16kHz;
        break;
      case 22050:
        wm8904Config.format.sampleRate = kWM8904_SampleRate22050Hz;
        break;
      case 24000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate24kHz;
        break;
      case 32000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate32kHz;
        break;
      case 44100:
        wm8904Config.format.sampleRate = kWM8904_SampleRate44100Hz;
        break;
      case 48000:
        wm8904Config.format.sampleRate = kWM8904_SampleRate48kHz;
        break;
      default:
        /* codec does not support this sample rate. */
        break;
    }
  }

  return CLOCK_GetMclkClkFreq();
}

void sbc_configured(struct bt_a2dp_codec_cfg* config) {
  audioTxConfig.sampleRate_Hz = bt_a2dp_sbc_get_sampling_frequency(
      (struct bt_a2dp_codec_sbc_params*)&config->codec_config->codec_ie[0]);
  audioTxConfig.lineChannels = (hal_audio_channel_t)bt_a2dp_sbc_get_channel_num(
      (struct bt_a2dp_codec_sbc_params*)&config->codec_config->codec_ie[0]);
  audioTxConfig.srcClock_Hz =
      BOARD_SwitchAudioFreq(audioTxConfig.sampleRate_Hz);

  PW_LOG_DEBUG("a2dp configure sample rate %dHz\r\n",
               (int)audioTxConfig.sampleRate_Hz);

  HAL_AudioTxInit((hal_audio_handle_t)&audio_tx_handle[0], &audioTxConfig);
  HAL_AudioTxInstallCallback(
      (hal_audio_handle_t)&audio_tx_handle[0], tx_callback, NULL);

  if (CODEC_Init(&codec_handle, &boardCodecConfig) != kStatus_Success) {
    PW_LOG_DEBUG("codec init failed!\r\n");
  }
  CODEC_SetMute(
      &codec_handle,
      kCODEC_PlayChannelHeadphoneRight | kCODEC_PlayChannelHeadphoneLeft,
      true);
  CODEC_SetFormat(&codec_handle,
                  audioTxConfig.srcClock_Hz,
                  audioTxConfig.sampleRate_Hz,
                  audioTxConfig.bitWidth);
  CODEC_SetVolume(&codec_handle, kCODEC_VolumeDAC, A2DP_CODEC_DAC_VOLUME);
  CODEC_SetVolume(&codec_handle,
                  kCODEC_VolumeHeadphoneLeft | kCODEC_VolumeHeadphoneRight,
                  A2DP_CODEC_HP_VOLUME);
  CODEC_SetMute(
      &codec_handle,
      kCODEC_PlayChannelHeadphoneRight | kCODEC_PlayChannelHeadphoneLeft,
      false);
}

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

static uint16_t jpl_callback_handle(
    unsigned char event_type,
    unsigned char* event_data,
    uint16_t event_datalen) {
  if ((JPL_UNDERFLOW_IND == event_type) ||
      (JPL_SILENCE_DATA_IND == event_type)) {
    // PW_LOG_DEBUG("Empty ");
  }
  hal_audio_transfer_t xfer;
  xfer.dataSize = event_datalen;
  xfer.data = event_data;

  if (kStatus_HAL_AudioSuccess !=
      HAL_AudioTransferSendNonBlocking((hal_audio_handle_t)&audio_tx_handle[0],
                                       &xfer)) {
    PW_LOG_DEBUG("failed to send audio data");
  }

  return API_SUCCESS;
}

void stream_started(struct bt_a2dp_stream* stream) {
  (void)stream;
  auto result = BT_jpl_init(jpl_callback_handle);
  if (result != API_SUCCESS) {
    PW_LOG_CRITICAL("JPL init failed");
    return;
  }
  JPL_BUFFERS buffer;
  buffer.sbc = a2dp_sbc_buffer;
  buffer.pcm = a2dp_pcm_buffer;
  buffer.silence = a2dp_silence_buffer;
  result = BT_jpl_register_buffers(&buffer);
  if (result != API_SUCCESS) {
    PW_LOG_CRITICAL("JPL register buffers failed");
    return;
  }
  result = BT_jpl_start(&jpl_param);
  if (result != API_SUCCESS) {
    PW_LOG_CRITICAL("JPL start failed");
    return;
  }

  PW_LOG_DEBUG("stream started");
}

void sink_sbc_streamer_data(struct bt_a2dp_stream* stream,
                            struct net_buf* buf,
                            uint16_t seq_num,
                            uint32_t ts) {
  (void)stream;
  auto retval =
      BT_jpl_add_frames(seq_num, ts, (buf->data.data()), (buf->len()));
  if (retval != API_SUCCESS) {
    PW_LOG_CRITICAL("JPL add frames failed");
    return;
  }
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
    PW_LOG_DEBUG("a2dp connected");
  } else {
    PW_LOG_DEBUG("a2dp connecting fail");
  }
}

void app_disconnected(struct bt_a2dp* a2dp) {
  (void)a2dp;
  PW_LOG_DEBUG("a2dp disconnected");
}

int app_config_req(struct bt_a2dp_ep* ep,
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

    sbc_configured(codec_cfg);
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
  bt_a2dp_register_cb(&a2dp_cb);
  PW_LOG_INFO(
      "bt_a2dp_register_ep: %d",
      bt_a2dp_register_ep(&sink_sbc_endpoint, BT_AVDTP_AUDIO, BT_AVDTP_SINK));
  bt_a2dp_init_zephyr();
  struct bt_conn* conn = nullptr;
  struct bt_avdtp* avdtp_session;

  auto handle = adapter->bredr()->RegisterService(
      records,
      kChannelParams,
      [&channels, &conn, &avdtp_session](auto channel,
                                         const auto& data_element) {
        (void)data_element;
        auto retval = a2dp_accept(conn, &avdtp_session);
        if (retval) {
          PW_LOG_CRITICAL("Failed to accept A2DP connection, aborting.");
          return;
        }
        avdtp_session->br_chan.chan.chan = channel;
        channel->Activate(
            [channel, avdtp_session](auto buffer) {
              avdtp_session->br_chan.chan.chan = channel;
              struct net_buf buf;
              for (unsigned int i = 0; i < buffer->size(); i++) {
                buf.data.push_back(buffer->data()[i]);
              }

              bt_avdtp_l2cap_recv(&avdtp_session->br_chan.chan, &buf);
            },
            [&channels, &channel]() {
                    auto chan = std::find_if(
                        channels.begin(), channels.end(),
                        [&channel](const auto& chan) { return chan->unique_id() == channel->unique_id(); });
                    if (chan != channels.end()) {
                      channels.erase(chan);
                    }
                    PW_LOG_INFO("Channel closed"); });
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
  pw::thread::DetachedThread(pw::thread::freertos::Options(),
                             frame_remove_task);
}

}  // namespace pw::system
