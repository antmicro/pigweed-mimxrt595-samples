#pragma once
#include "pw_bytes/span.h"
#include "pw_fastboot/fastboot_device.h"

namespace bootloader {

struct Partition {
  uint32_t start;
  uint32_t length;
  char name[16];
};

bool DoFlash(pw::fastboot::Device*, std::string);

}  // namespace bootloader
