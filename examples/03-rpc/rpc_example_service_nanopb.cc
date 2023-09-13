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

#include "rpc_example/rpc_example_service_nanopb.h"

#include <mutex>

#include "pw_log/log.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace rpc_example {

pw::Status DeviceInfoService::SetUuid(const rpc_example_DeviceUuid& request,
                                      pw_protobuf_Empty&) {
  std::lock_guard lock(uuid_lock_);
  if (request.uuid.size > kUuidSizeBytes) {
    PW_LOG_ERROR("Received a UUID larger than the max UUID size");
    return pw::Status::ResourceExhausted();
  }
  memcpy(uuid_.data(), request.uuid.bytes, request.uuid.size);
  current_uuid_size_ = request.uuid.size;
  return pw::OkStatus();
}

pw::Status DeviceInfoService::GetUuid(const pw_protobuf_Empty&,
                                      rpc_example_DeviceUuid& response) {
  std::lock_guard lock(uuid_lock_);
  if (current_uuid_size_ == 0) {
    PW_LOG_ERROR("UUID request received, but this device has no UUID yet");
    return pw::Status::NotFound();
  }
  if (current_uuid_size_ > kUuidSizeBytes) {
    PW_LOG_ERROR("Current UUID larger than the max UUID size");
    return pw::Status::ResourceExhausted();
  }
  memcpy(response.uuid.bytes, uuid_.data(), current_uuid_size_);
  response.uuid.size = current_uuid_size_;
  return pw::OkStatus();
}

}  // namespace rpc_example
