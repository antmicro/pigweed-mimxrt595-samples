#include "pw_fastboot/device_hal.h"
#include "pw_fastboot/device_variable.h"
#include "pw_fastboot/transport.h"
#include "transport_mimxrt595evk.h"
#include "pw_thread/detached_thread.h"
#include "pw_fastboot/fastboot_device.h"
#include <cstdio>
#include <memory>
#include <string>

static void FastbootProtocolLoop() {
    auto variables = std::make_unique<pw::fastboot::VariableProvider>();
    variables->RegisterVariable("test1", [](auto, auto, std::string* message) -> bool {
        *message = "VARIABLE1";
        return true;
    });
    variables->RegisterVariable("test2", [](auto, auto, std::string* message) -> bool {
        *message = "TEST-VARIABLE-2";
        return true;
    });
    variables->RegisterSpecialVariable("test3", [](pw::fastboot::Device* device) -> bool {
        device->WriteInfo("This is an example of a special/multiline fastboot variable.");
        device->WriteInfo("This can be used for example to dump logs.");
        device->WriteInfo("To write a line, call device->WriteInfo.");
        device->WriteInfo("Special variables are not displayed in output of fastboot getvar all.");
        return true;
    });
    pw::fastboot::Device device {std::make_unique<fastboot::mimxrt595evk::UsbTransport>(),
                           std::move(variables),
                           std::make_unique<pw::fastboot::DeviceHAL>()};
    device.ExecuteCommands();
}

namespace pw::system {

void UserAppInit() {
    // Start new thread as WorkQueue thread stack is limited to 512.
    pw::thread::DetachedThread(pw::thread::freertos::Options()
        .set_name("fastboot"), FastbootProtocolLoop);
}

}  // namespace pw::system