#include "pw_log/log.h"

namespace pw::system {

void UserAppInit() {
  PW_LOG_INFO("Hello world from user app!");
  PW_LOG_INFO("This should be booted from an alternative flash location.");
}

}  // namespace pw::system