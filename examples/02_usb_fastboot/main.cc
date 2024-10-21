#include <cstdio>
#include <memory>
#include <string>

#include "pw_fastboot/device_hal.h"
#include "pw_fastboot/device_variable.h"
#include "pw_fastboot/fastboot_device.h"
#include "pw_fastboot_usb_mcuxpresso/transport.h"
#include "pw_log/log.h"
#include "pw_status/status.h"
#include "pw_thread/detached_thread.h"

// Enable entering of fastboot using SW1
#define FASTBOOT_ENABLE_GPIO (1)

enum class BootMode : std::uint32_t {
  User = 1,
  Fastboot = 2,
};
struct BootData {
  static constexpr std::uint32_t BOOTLOADER_DATA_MAGIC =
      0x54534146;  // `FAST` string, little-endian
  std::uint32_t magic;
  BootMode boot_mode;

  constexpr bool valid() const { return magic == BOOTLOADER_DATA_MAGIC; }

  constexpr void set(BootMode mode) {
    magic = BOOTLOADER_DATA_MAGIC;
    boot_mode = mode;
  }

  constexpr void clear() { magic = 0x0; }
};
#define BOOTDATA reinterpret_cast<BootData*>(FASTBOOT_BOOT_DATA_BASE)

#if defined(FASTBOOT_ENABLE_GPIO) && FASTBOOT_ENABLE_GPIO > 0

#include "board.h"
#include "config/pin_mux.h"
#include "fsl_power.h"
#include "pw_digital_io/digital_io.h"
#include "pw_digital_io_mcuxpresso/digital_io.h"

static bool IsGpioLow(pw::digital_io::DigitalIn& pin) {
  if (const auto err = pin.Enable(); err != PW_STATUS_OK) {
    PW_LOG_ERROR("Failed to enable GPIO with status: %d", err.code());
    return false;
  }

  const auto gpio = pin.GetState()
                        .or_else([](pw::Status s) {
                          PW_LOG_ERROR("Failed GPIO with status: %d", s.code());
                          return pw::digital_io::State::kInactive;
                        })
                        .value();

  return (gpio == pw::digital_io::State::kInactive);
}

static bool IsFastbootTriggered() {
  BOARD_InitBUTTONPins();
  pw::digital_io::McuxpressoDigitalIn in{
      BOARD_SW1_GPIO, BOARD_SW1_GPIO_PORT, BOARD_SW1_GPIO_PIN};
  return IsGpioLow(in);
}

[[noreturn]] static void ResetBoard() {
  asm volatile("cpsid if");

  SYSCTL0->PDRUNCFG0_CLR = SYSCTL0_PDRUNCFG0_LPOSC_PD_MASK;
  CLOCK_AttachClk(kLPOSC_to_WDT0_CLK);
  POWER_DisablePD(kPDRUNCFG_PD_LPOSC); /* Power on LPOSC (1MHz) */
  CLOCK_EnableLpOscClk();              /* Wait until LPOSC stable */
#if !defined(FSL_FEATURE_WWDT_HAS_NO_PDCFG) || (!FSL_FEATURE_WWDT_HAS_NO_PDCFG)
  POWER_DisablePD(kPDRUNCFG_PD_LPOSC);
#endif

  CLOCK_EnableClock(kCLOCK_Wwdt0);
  RESET_PeripheralReset(kWWDT0_RST_SHIFT_RSTn);
  WWDT0->TC = 0xFF;  // minimum value is 0xFF
  WWDT0->MOD = 0x3;  // enabled, will reset on expiration (not yet running)
  WWDT0->WINDOW = 0xFF;
  WWDT0->WARNINT = 0xFF;
  WWDT0->FEED = 0xAA;
  WWDT0->FEED = 0x55;  // watchdog is now running
  while (true) {
    // wait until the watchdog resets the board
  }
}
#endif

extern "C" {
// This function is called from pw_boot_PreStaticMemoryInit, after CMSIS'
// SystemInit completes. This is slightly unsafe, as we need the boot state
// to be as close to out-of-reset as possible when jumping to the user code.
// However, SystemInit currently only performs very basic cache configuration
// (as opposed to initializing entire peripherals at a time), so at the moment
// this is fine.
void SystemInitHook() {
#if defined(FASTBOOT_ENABLE_GPIO) && FASTBOOT_ENABLE_GPIO > 0
  // For debugging, forcing fastboot using GPIO is very handy, but requires
  // that we perform a lot of clock initialization that may affect the user
  // application later. This sets the boot mode in BOOTDATA using the GPIO
  // state and resets the board to ensure sane state.
  if (!BOOTDATA->valid()) {
    const auto fastboot_gpio_triggered = IsFastbootTriggered();
    BOOTDATA->set(fastboot_gpio_triggered ? BootMode::Fastboot
                                          : BootMode::User);
    ResetBoard();
  }
#endif

  const auto boot_mode =
      BOOTDATA->valid() ? BOOTDATA->boot_mode : BootMode::User;
  BOOTDATA->clear();

  switch (boot_mode) {
    default:
    case BootMode::Fastboot: {
      // Return to pw_boot_cortex_m, which will initialize the fastboot
      // application
      return;
    }
    case BootMode::User: {
      // Jump to the user application
      struct vector_table {
        std::size_t msp;
        std::size_t reset;
      };
      auto* vt =
          reinterpret_cast<struct vector_table*>(FASTBOOT_APP_VECTOR_TABLE);
      (reinterpret_cast<void (*)(void)>(vt->reset))();
      // If this returns, the user app was corrupted. This will
      // never happen in regular circumstances, as the very first thing
      // the app must do is relocate the VTOR and configure its own
      // stack pointer.
      while (true) {
        // explicit hang
      }
    }
  }
}
}

static void FastbootProtocolLoop() {
  auto variables = std::make_unique<pw::fastboot::VariableProvider>();
  variables->RegisterVariable("test1",
                              [](auto, auto, std::string* message) -> bool {
                                *message = "VARIABLE1";
                                return true;
                              });
  variables->RegisterVariable("test2",
                              [](auto, auto, std::string* message) -> bool {
                                *message = "TEST-VARIABLE-2";
                                return true;
                              });
  variables->RegisterSpecialVariable(
      "test3", [](pw::fastboot::Device* device) -> bool {
        device->WriteInfo(
            "This is an example of a special/multiline fastboot variable.");
        device->WriteInfo("This can be used for example to dump logs.");
        device->WriteInfo("To write a line, call device->WriteInfo.");
        device->WriteInfo(
            "Special variables are not displayed in output of fastboot getvar "
            "all.");
        return true;
      });
  pw::fastboot::Device device{pw::fastboot::CreateMimxrt595UsbTransport(),
                              std::move(variables),
                              std::make_unique<pw::fastboot::DeviceHAL>()};
  device.ExecuteCommands();
}

namespace pw::system {

void UserAppInit() {
  // Start new thread as WorkQueue thread stack is limited to 512.
  pw::thread::DetachedThread(
      pw::thread::freertos::Options().set_name("fastboot"),
      FastbootProtocolLoop);
}

}  // namespace pw::system
