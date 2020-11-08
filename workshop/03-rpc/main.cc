// Copyright 2020 The Pigweed Authors
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

#include <array>
#include <span>
#include <string_view>

#include "pw_board_led/led.h"
#include "pw_hdlc_lite/encoder.h"
#include "pw_hdlc_lite/rpc_channel.h"
#include "pw_hdlc_lite/rpc_packets.h"
#include "pw_hdlc_lite/sys_io_stream.h"
#include "pw_log/log.h"
#include "pw_rpc/echo_service_nanopb.h"
#include "pw_rpc/server.h"
#include "pw_spin_delay/delay.h"
// TODO FOR WORKSHOP: Include your RPC service declaration here!

// ------------------- pw_rpc subsystem setup -------------------
// There are multiple ways to plumb pw_rpc in your product. In the future,
// Pigweed may offer an optional pre-canned setup; but for now, you must
// manually snap together the modular pieces.
//
// The RPC system in this code is layered as:
//
//   UART --> pw_sys_io ------> hdlc -------> pw_rpc
//   (phy)                   (transport)
//
// HDLC converts the raw UART/serial byte stream into a packet stream. Then RPC
// operates at the packet level.
//
// This is just one way to configure pw_rpc, which is designed to be flexible
// and work over wh atever physical or logical transport you have available.

constexpr size_t kMaxTransmissionUnit = 256;  // bytes

// Used to write HDLC data to pw::sys_io. This is an implementation of the
// pw::stream::Stream interface.
pw::stream::SysIoWriter writer;

// Set up the output channel for the pw_rpc server to use. This one happens to
// implement the packet in / packet out with HDLC. pw_rpc can use any
// ChannelOptput implementation, including custom ones for your product.
pw::hdlc_lite::RpcChannelOutputBuffer<kMaxTransmissionUnit> hdlc_channel_output(
    writer, pw::hdlc_lite::kDefaultRpcAddress, "HDLC channel");

// A pw::rpc::Server can have multiple channels (e.g. a UART and a BLE
// connection). In this case, there is only one (HDLC over UART).
pw::rpc::Channel channels[] = {
    pw::rpc::Channel::Create<1>(&hdlc_channel_output)};

// Declare the pw_rpc server with the HDLC channel.
pw::rpc::Server server(channels);

// Declare a buffer for decoding incoming HDLC frames.
std::array<std::byte, kMaxTransmissionUnit> input_buffer;

// Decoder object consumes bytes and return if a HDLC packet was completed.
pw::hdlc_lite::Decoder hdlc_decoder(input_buffer);

// ------------------- pw_rpc service registration  -------------------
pw::rpc::EchoService echo_service;

// TODO FOR WORKSHOP: Declare your service object here!

void RegisterServices() {
  server.RegisterService(echo_service);
  // TODO FOR WORKSHOP: Register your service here!
}

// ------------------- pw_rpc  -------------------

constexpr unsigned kHdlcChannelForRpc = pw::hdlc_lite::kDefaultRpcAddress;
constexpr unsigned kHdlcChannelForLogs = 1;

void ParseByteFromUartAndHandleRpcs() {
  // Read a byte from the UART if one is available; if not, bail.
  std::byte data;
  if (!pw::sys_io::TryReadByte(&data).ok()) {
    return;
  }

  // Byte received. Send the byte to the HDLC decoder; see if a packet finished.
  auto result = hdlc_decoder.Process(data);

  // Packet didn't parse correctly, so ignore it. In production, this should
  // perhaps log or increment a metric (see pw_metric) to track bad packets.
  if (!result.ok()) {
    // POST-WORKSHOP EXERCISE: Add a tracking metric for bad packets, and
    // expose the metric via the pw_metric RPC service. This will require
    // making some metrics objects, incrementing them; then creating and
    // registering a metric RPC service.
    //
    // See https://pigweed.dev/pw_metric/
    //     https://pigweed.dev/pw_metric/#exporting-metrics
    return;
  }

  PW_LOG_INFO("Got complete HDLC packet");

  // A frame was completed.
  pw::hdlc_lite::Frame& hdlc_frame = result.value();
  if (hdlc_frame.address() != kHdlcChannelForRpc) {
    // We ignore frames that are for unknown addresses, but you could put
    // some code here if you wanted to stream custom data from PC --> device.
    PW_LOG_WARN("Got packet with no destination; address: %d",
                hdlc_frame.address());
    return;
  }

  pw::board_led::Toggle();

  // Packet was validated and correct (CRC, etc); so send it to the RPC server.
  // The RPC server may send response packets before returning from this call.
  server.ProcessPacket(hdlc_frame.data(), hdlc_channel_output);
}

// TODO FOR WORKSHOP: Add an RPC to change the blink time.
int state = 0;
int counter = 0;
int counter_max = 5'000'000;
void Blink() {
  // Toggle the state if needed.
  counter += 1;
  // PW_LOG_INFO("Counter: %d", counter);
  if (counter < counter_max) {
    // Haven't hit a toggle event yet; bail.
    return;
  }
  state = 1 - state;
  counter = 0;

  if (state == 0) {
    PW_LOG_INFO("Blink High!");
    pw::board_led::TurnOn();
  } else {
    PW_LOG_INFO("Blink Low!");
    pw::board_led::TurnOff();
  }
}

// TODO FOR WORKSHOP: Why doesn't this work instead of Blink() above?
void BlinkNoWorky() {
  PW_LOG_INFO("Blink High!");
  pw::board_led::TurnOn();
  pw::spin_delay::WaitMillis(1000);

  PW_LOG_INFO("Blink Low!");
  pw::board_led::TurnOff();
  pw::spin_delay::WaitMillis(1000);
}

int main() {
  pw::board_led::Init();

  PW_LOG_INFO("Registering pw_rpc services");
  RegisterServices();

  // Superloop!
  while (true) {
    // Toggle the LED if needed.
    Blink();

    // Examine incoming serial byte; if a packet finished, send it to RPC.
    ParseByteFromUartAndHandleRpcs();
  }

  return 0;
}
