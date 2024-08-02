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

#include "pw_chrono/system_clock.h"
#include "pw_chrono/system_timer.h"
#include "pw_log/log.h"
#include "pw_system/work_queue.h"
#include "pw_bluetooth/host.h"

#include "bluetooth/bluetooth.h"
#include "controller_hci_uart.h"

int controller_hci_uart_get_configuration(controller_hci_uart_config_t *config)
{
    if (NULL == config)
    {
        return -1;
    }
    config->clockSrc        = 0u;
    config->defaultBaudrate = 115200u;
    config->runningBaudrate = 0u;
    config->instance        = 0u;
    config->enableRxRTS     = 1u;
    config->enableTxCTS     = 1u;

    return 0;
}

static void bt_ready(int err) {
    (void)err;
    PW_LOG_INFO("Bluetooth ready");
}


namespace pw::bluetooth {
using namespace pw::bluetooth;
class MimxrtHost : public Host {

public:
    MimxrtHost() : Host() {
        bt_enable(bt_ready);
    }

    void Initialize(Controller* controller, PersistentData data, Function<void(Status)>&& on_initialization_complete) override {
        (void)controller;
        (void)data;
        (void)on_initialization_complete;
        PW_LOG_INFO("Initializing host");
    }

    void Close(Closure callback) override {
        (void)callback;
        PW_LOG_INFO("Closing host");
    }

    low_energy::Central* Central() override {
        PW_LOG_INFO("Returning central");
        return nullptr;
    }

    low_energy::Peripheral* Peripheral() override {
        PW_LOG_INFO("Returning peripheral");
        return nullptr;
    }

    gatt::Server* GattServer() override {
        PW_LOG_INFO("Returning GATT server");
        return nullptr;
    }

    Status ForgetPeer(PeerId peer_id) override {
        (void)peer_id;
        PW_LOG_INFO("Forgetting peer");
        return PW_STATUS_UNKNOWN;
    }

    void EnablePrivacy(bool enable) override {
        (void)enable;
        PW_LOG_INFO("Enabling privacy");
    }

    void SetSecurityMode(low_energy::SecurityMode mode) override {
        (void)mode;
        PW_LOG_INFO("Setting security mode");
    }

    void SetPairingDelegate(InputCapability input, OutputCapability output, PairingDelegate* delegate) override {
        (void)input;
        (void)output;
        (void)delegate;
        PW_LOG_INFO("Setting pairing delegate");
    }

    void Pair(PeerId peer_id, PairingOptions options, Function<void(Status)>&& callback) override {
        (void)peer_id;
        (void)options;
        (void)callback;
        PW_LOG_INFO("Pairing with peer");
    }

    void SetBondDataCallback(Function<void(low_energy::BondData)>&& callback) override {
        (void)callback;
        PW_LOG_INFO("Setting bond data callback");
    }

    std::optional<PeerId> PeerIdFromAddress(Address address) override {
        (void)address;
        PW_LOG_INFO("Getting peer ID from address");
        return std::nullopt;
    }

    std::optional<Address> DeviceAddressFromPeerId(PeerId peer_id) override {
        (void)peer_id;
        PW_LOG_INFO("Getting device address from peer ID");
        return std::nullopt;
    }
private:
};
}

namespace pw::system {

void UserAppInit() {
    //pw::bluetooth::Controller controller;
    //pw::bluetooth::Host::PersistentData data;
    pw::bluetooth::MimxrtHost host;

    //host.Initialize(&controller, data);

    PW_LOG_CRITICAL("Hello, world!");
}

}  // namespace pw::system
