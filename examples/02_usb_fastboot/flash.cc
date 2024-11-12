#include "flash.h"

#include <cstddef>
#include <cstring>
#include <optional>

#include "pw_bytes/span.h"
#include "pw_flash/flash.h"
#include "pw_flash_mcuxpresso/flash.h"
#include "pw_log/log.h"
#include "pw_status/status.h"

// Partitions supported for flashing
// Start and length specify the bounds of the partition, the given
// name can be directly used to flash the partition using fastboot.
static constexpr bootloader::Partition kPartitions[] = {
    {FASTBOOT_BOOTLOADER_BEGIN, FASTBOOT_BOOTLOADER_SIZE, "fastboot"},
    {FASTBOOT_APP_VECTOR_TABLE, FASTBOOT_APP_SIZE, "system"},
};

static std::optional<bootloader::Partition> FindPartitionByName(
    std::string name) {
  auto parts = std::span{kPartitions};
  auto it = std::find_if(
      parts.begin(), parts.end(), [name](bootloader::Partition const& v) {
        return std::strcmp(v.name, name.c_str()) == 0;
      });
  return it != parts.end() ? std::optional{*it} : std::nullopt;
}

static bool ErasePartition(pw::flash::Flash& flash,
                           bootloader::Partition const& part,
                           pw::ConstByteSpan image) {
  const auto err =
      flash.Erase(pw::flash::Range{part.start, image.size_bytes()});
  if (!err.ok() && !err.IsDataLoss()) {
    PW_LOG_ERROR("Erasing flash failed with error code %d", err.code());
    return false;
  }
  return true;
}

static bool WritePartition(pw::flash::Flash& flash,
                           bootloader::Partition const& part,
                           pw::ConstByteSpan image) {
  const auto err =
      flash.Program(pw::flash::Range{part.start, image.size_bytes()}, image);
  if (!err.ok()) {
    PW_LOG_ERROR("Programming flash failed with error code %d", err.code());
    return false;
  }
  return true;
}

bool bootloader::DoFlash(pw::fastboot::Device* device, std::string name) {
  auto maybe_part = FindPartitionByName(name);
  if (!maybe_part.has_value()) {
    device->WriteFail("partition does not exist");
    return false;
  }
  auto partition = maybe_part.value();

  auto image = pw::ConstByteSpan{
      reinterpret_cast<std::byte*>(device->download_data().data()),
      device->download_data().size()};

  auto flash = pw::flash::McuxpressoFlash();
  if (!flash.Initialize().ok()) {
    device->WriteFail("flash init failed");
    return false;
  }

  if (!ErasePartition(flash, partition, image)) {
    device->WriteFail("erase failed");
    return false;
  }

  if (!WritePartition(flash, partition, image)) {
    device->WriteFail("write failed");
    return false;
  }

  PW_LOG_INFO("Flashing completed!");
  return true;
}
