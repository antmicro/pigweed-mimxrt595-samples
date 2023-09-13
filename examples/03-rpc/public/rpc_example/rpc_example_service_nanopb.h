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
#pragma once

#include <array>

#include "pw_status/status.h"
#include "pw_sync/mutex.h"
#include "rpc_example_protos/rpc_example_service.rpc.pb.h"

namespace rpc_example {

class DeviceInfoService final
    : public ::rpc_example::pw_rpc::nanopb::DeviceInfo::Service<
          DeviceInfoService> {
 public:
  DeviceInfoService() = default;

  pw::Status SetUuid(const rpc_example_DeviceUuid& request,
                     pw_protobuf_Empty& response);
  pw::Status GetUuid(const pw_protobuf_Empty& request,
                     rpc_example_DeviceUuid& response);

 private:
  static constexpr size_t kUuidSizeBytes =
      sizeof(rpc_example_DeviceUuid_uuid_t().bytes);
  size_t current_uuid_size_ = 0;
  std::array<std::byte, kUuidSizeBytes> uuid_;
  pw::sync::Mutex uuid_lock_;
};

}  // namespace rpc_example
