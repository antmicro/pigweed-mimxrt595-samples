#include "board.h"
#include "bootloader.h"
#include "pw_log/log.h"
#include "pw_thread/sleep.h"

namespace pw::system {

void UserAppInit() {
  PW_LOG_INFO("Hello world from user app!");
  PW_LOG_INFO("This should be booted from an alternative flash location.");
  PW_LOG_INFO("Rebooting into fastboot in 4 seconds...");
  pw::this_thread::sleep_for(std::chrono::seconds(4));
  BOOTDATA->set(BootMode::Fastboot);
  NVIC_SystemReset();
}

}  // namespace pw::system