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
#include <string_view>

#include "pw_assert/check.h"
#include "pw_hdlc/encoder.h"
#include "pw_hdlc/rpc_channel.h"
#include "pw_hdlc/rpc_packets.h"
#include "pw_log/log.h"
#include "pw_rpc/echo_service_nanopb.h"
#include "pw_rpc/server.h"
#include "pw_span/span.h"
#include "pw_stream/sys_io_stream.h"
#include "simple_counter/simple_counter.h"

namespace {
// Max number of bytes sent in an RPC packet.
constexpr size_t kMaxTransmissionUnit = 256;

// Used to write HDLC data to pw::sys_io.
pw::stream::SysIoWriter writer;

// Set up the output channel for the pw_rpc server to use to use.
pw::hdlc::RpcChannelOutput hdlc_channel_output(writer,
                                               pw::hdlc::kDefaultRpcAddress,
                                               "HDLC channel");

pw::rpc::Channel channels[] = {
    pw::rpc::Channel::Create<1>(&hdlc_channel_output)};

// Declare the pw_rpc server with the HDLC channel.
pw::rpc::Server server(channels);

pw::rpc::EchoService echo_service;
}  // namespace

void RegisterServices() { server.RegisterService(echo_service); }

// Creates and runs an RPC server, with an echo service echoing data from RPCs.
void CreateAndRunRpcServerWithEchoService() {
  // Set up the server and start processing data.
  RegisterServices();

  // Declare a buffer for decoding incoming HDLC frames.
  std::array<std::byte, kMaxTransmissionUnit> input_buffer;

  pw::hdlc::ReadAndProcessPackets(
      server, hdlc_channel_output, input_buffer, pw::hdlc::kDefaultRpcAddress);
}

int main() {
  // Use sample Module
  sample::SimpleCounter counter(3);
  counter.Add(-1);
  // Sample assert. Adding a message is optional.
  PW_CHECK_INT_EQ(counter.GetCount(), 2);
  counter.Add(10);

  // Chance this value to see the assert below crash the app.
  const int expected_count = 12;

  // Use sample assert with a message.
  PW_CHECK_INT_EQ(counter.GetCount(),
                  expected_count,
                  "Counter value %d != %d as expected",
                  counter.GetCount(),
                  expected_count);

  PW_LOG_INFO("Tokenize strings 'cause");
  PW_LOG_DEBUG("logs create bloated binaries");
  PW_LOG_CRITICAL("this string does not count.");

  // Loops processing RPCs.
  CreateAndRunRpcServerWithEchoService();

  return 0;
}
