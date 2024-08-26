#include "transport_mimxrt595evk.h"
#include "pw_log/log.h"
#include "pw_thread/detached_thread.h"

static void FastbootProtocolLoop() {
    static std::byte s_packet_buffer[512];
    static constexpr char FASTBOOT_RESPONSE_OKAY[] = "OKAY";

    fastboot::mimxrt595evk::UsbTransportInit();
    while (true) {
        auto n = fastboot::mimxrt595evk::FastbootReceivePacket(pw::ByteSpan{s_packet_buffer, sizeof(s_packet_buffer)});
        (void)n;
        fastboot::mimxrt595evk::FastbootSendPacket(pw::ConstByteSpan{(std::byte const*)FASTBOOT_RESPONSE_OKAY, sizeof(FASTBOOT_RESPONSE_OKAY)});
    }
}

namespace pw::system {

void UserAppInit() {
    // Start new thread as WorkQueue thread stack is limited to 512.
    pw::thread::DetachedThread(pw::thread::freertos::Options()
        .set_name("fastboot"), FastbootProtocolLoop);
}

}  // namespace pw::system