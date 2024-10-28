#include <cstdio>
#include <memory>
#include <string>

#include "board.h"
#include "bootloader.h"
#include "pw_fastboot/device_hal.h"
#include "pw_fastboot/device_variable.h"
#include "pw_fastboot/fastboot_device.h"
#include "pw_fastboot_usb_mcuxpresso/transport.h"
#include "pw_log/log.h"
#include "pw_status/status.h"
#include "pw_thread/detached_thread.h"

// Enable entering of fastboot using SW1
#define FASTBOOT_ENABLE_GPIO (1)

#if defined(FASTBOOT_ENABLE_GPIO) && FASTBOOT_ENABLE_GPIO > 0

#include "config/pin_mux.h"
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

#endif

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

static void CleanUpAfterBootloader() {
  // Disable interrupts
  asm volatile("cpsid i");

  // Disable SYSTICK timer
  // FreeRTOS support enables the SYSTICK, and its configuration will carry
  // over to the user application, which will cause faults if it did not expect
  // an interrupt to occur.
  SysTick->CTRL = 0x0;

  // Put all peripherals into reset before jumping to the user application.
  // Enumeration values were taken directly from _RSTCTL_RSTn enum in
  // `fsl_reset.h`.
  // FlexSPI0 is excluded, as it must be initialized (the application is
  // executed in place from the flash).
  constexpr RSTCTL_RSTn_t PERIPHERALS_TO_RESET[] = {
      kDSP_RST_SHIFT_RSTn,           kAXI_SWITCH_RST_SHIFT_RSTn,
      kPOWERQUAD_RST_SHIFT_RSTn,     kCASPER_RST_SHIFT_RSTn,
      kHASHCRYPT_RST_SHIFT_RSTn,     kPUF_RST_SHIFT_RSTn,
      kRNG_RST_SHIFT_RSTn, /*kFLEXSPI0_RST_SHIFT_RSTn,*/
      kFLEXSPI1_RST_SHIFT_RSTn,      kUSBHS_PHY_RST_SHIFT_RSTn,
      kUSBHS_DEVICE_RST_SHIFT_RSTn,  kUSBHS_HOST_RST_SHIFT_RSTn,
      kUSBHS_SRAM_RST_SHIFT_RSTn,    kSCT_RST_SHIFT_RSTn,
      kGPU_RST_SHIFT_RSTn,           kDISP_CTRL_RST_SHIFT_RSTn,
      kMIPI_DSI_CTRL_RST_SHIFT_RSTn, kMIPI_DSI_PHY_RST_SHIFT_RSTn,
      kSMART_DMA_RST_SHIFT_RSTn,     kSDIO0_RST_SHIFT_RSTn,
      kSDIO1_RST_SHIFT_RSTn,         kACMP0_RST_SHIFT_RSTn,
      kADC0_RST_SHIFT_RSTn,          kSHSGPIO0_RST_SHIFT_RSTn,
      kUTICK0_RST_SHIFT_RSTn,        kWWDT0_RST_SHIFT_RSTn,
      kFC0_RST_SHIFT_RSTn,           kFC1_RST_SHIFT_RSTn,
      kFC2_RST_SHIFT_RSTn,           kFC3_RST_SHIFT_RSTn,
      kFC4_RST_SHIFT_RSTn,           kFC5_RST_SHIFT_RSTn,
      kFC6_RST_SHIFT_RSTn,           kFC7_RST_SHIFT_RSTn,
      kFC8_RST_SHIFT_RSTn,           kFC9_RST_SHIFT_RSTn,
      kFC10_RST_SHIFT_RSTn,          kFC11_RST_SHIFT_RSTn,
      kFC12_RST_SHIFT_RSTn,          kFC13_RST_SHIFT_RSTn,
      kFC14_RST_SHIFT_RSTn,          kFC15_RST_SHIFT_RSTn,
      kDMIC_RST_SHIFT_RSTn,          kFC16_RST_SHIFT_RSTn,
      kOSEVENT_TIMER_RST_SHIFT_RSTn, kFLEXIO_RST_SHIFT_RSTn,
      kHSGPIO0_RST_SHIFT_RSTn,       kHSGPIO1_RST_SHIFT_RSTn,
      kHSGPIO2_RST_SHIFT_RSTn,       kHSGPIO3_RST_SHIFT_RSTn,
      kHSGPIO4_RST_SHIFT_RSTn,       kHSGPIO5_RST_SHIFT_RSTn,
      kHSGPIO6_RST_SHIFT_RSTn,       kHSGPIO7_RST_SHIFT_RSTn,
      kCRC_RST_SHIFT_RSTn,           kDMAC0_RST_SHIFT_RSTn,
      kDMAC1_RST_SHIFT_RSTn,         kMU_RST_SHIFT_RSTn,
      kSEMA_RST_SHIFT_RSTn,          kFREQME_RST_SHIFT_RSTn,
      kCT32B0_RST_SHIFT_RSTn,        kCT32B1_RST_SHIFT_RSTn,
      kCT32B2_RST_SHIFT_RSTn,        kCT32B3_RST_SHIFT_RSTn,
      kCT32B4_RST_SHIFT_RSTn,        kMRT0_RST_SHIFT_RSTn,
      kWWDT1_RST_SHIFT_RSTn,         kI3C0_RST_SHIFT_RSTn,
      kI3C1_RST_SHIFT_RSTn,          kPINT_RST_SHIFT_RSTn,
      kINPUTMUX_RST_SHIFT_RSTn,
  };
  for (auto peri : PERIPHERALS_TO_RESET) {
    RESET_SetPeripheralReset(peri);
  }
}

namespace pw::system {

void UserAppInit() {
#if defined(FASTBOOT_ENABLE_GPIO) && FASTBOOT_ENABLE_GPIO > 0
  if (!BOOTDATA->valid()) {
    const auto fastboot_gpio_triggered = IsFastbootTriggered();
    BOOTDATA->set(fastboot_gpio_triggered ? BootMode::Fastboot
                                          : BootMode::User);
  }
#endif

  const auto boot_mode =
      BOOTDATA->valid() ? BOOTDATA->boot_mode : BootMode::User;
  BOOTDATA->clear();

  switch (boot_mode) {
    default:
    case BootMode::Fastboot: {
      // Start new thread as WorkQueue thread stack is limited to 512.
      pw::thread::DetachedThread(
          pw::thread::freertos::Options().set_name("fastboot"),
          FastbootProtocolLoop);
      return;
    }
    case BootMode::User: {
      // Make sure that interrupts are disabled and the bootloader
      // won't cause any spurious interrupts for the user application
      // which it may not be able to handle. This also re-disables
      // peripherals that may have been initialized by the bootloader,
      // which the user app may not need.
      CleanUpAfterBootloader();

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

}  // namespace pw::system
