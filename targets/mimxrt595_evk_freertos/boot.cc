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

#include "pw_boot/boot.h"

#include "FreeRTOS.h"
#include "config/board.h"
#include "clock_config.h"
#include "controller_hci_uart.h"
#include "fsl_clock.h"
#include "fsl_debug_console.h"
#include "fsl_power.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "pw_boot_cortex_m/boot.h"
#include "pw_preprocessor/compiler.h"
#include "pw_sys_io_mcuxpresso/init.h"
#include "pw_system/init.h"
#include "task.h"

#if PW_MALLOC_ACTIVE
#include "pw_malloc/malloc.h"
#endif  // PW_MALLOC_ACTIVE

void pw_boot_PreStaticMemoryInit() {
  // Call CMSIS SystemInit code.
  SystemInit();
}

void pw_boot_PreStaticConstructorInit() {
#if PW_MALLOC_ACTIVE
  pw_MallocInit(&pw_boot_heap_low_addr, &pw_boot_heap_high_addr);
#endif  // PW_MALLOC_ACTIVE
}

#if (((defined(CONFIG_BT_SMP)) && (CONFIG_BT_SMP)))
#include "ksdk_mbedtls.h"
#endif /* CONFIG_BT_SMP */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*work as debug console with M.2, work as bt usart with non-M.2*/
#define APP_DEBUG_UART_TYPE kSerialPort_Uart
#define APP_DEBUG_UART_BASEADDR (uint32_t)USART12
#define APP_DEBUG_UART_INSTANCE 12U
#define APP_DEBUG_UART_CLK_FREQ CLOCK_GetFlexcommClkFreq(12)
#define APP_DEBUG_UART_FRG_CLK                                 \
  (const clock_frg_clk_config_t){                              \
      12U, clock_frg_clk_config_t::kCLOCK_FrgPllDiv, 255U, 0U} \
  /*!< Select FRG0 mux as frg_pll */
#define APP_DEBUG_UART_CLK_ATTACH kFRG_to_FLEXCOMM12
#define APP_UART_IRQ_HANDLER FLEXCOMM12_IRQHandler
#define APP_UART_IRQ FLEXCOMM12_IRQn
#define APP_DEBUG_UART_BAUDRATE 115200

int controller_hci_uart_get_configuration(
    controller_hci_uart_config_t* config) {
  if (config == nullptr) {
    return -1;
  }
  config->clockSrc = BOARD_BT_UART_CLK_FREQ;
  config->defaultBaudrate = 115200u;
  config->runningBaudrate = BOARD_BT_UART_BAUDRATE;
  config->instance = BOARD_BT_UART_INSTANCE;
  config->enableRxRTS = 1u;
  config->enableTxCTS = 1u;
  return 0;
}

void APP_InitAppDebugConsole(void) {
  uint32_t uartClkSrcFreq;

  /* attach FRG0 clock to FLEXCOMM12 (debug console) */
  auto debug_uart_cfg = APP_DEBUG_UART_FRG_CLK;
  CLOCK_SetFRGClock(&debug_uart_cfg);
  CLOCK_AttachClk(APP_DEBUG_UART_CLK_ATTACH);

  uartClkSrcFreq = APP_DEBUG_UART_CLK_FREQ;

  DbgConsole_Init(APP_DEBUG_UART_INSTANCE,
                  APP_DEBUG_UART_BAUDRATE,
                  APP_DEBUG_UART_TYPE,
                  uartClkSrcFreq);
}

void pw_boot_PreMainInit() {

  RESET_ClearPeripheralReset(kHSGPIO0_RST_SHIFT_RSTn);
  RESET_ClearPeripheralReset(kHSGPIO3_RST_SHIFT_RSTn);
  RESET_ClearPeripheralReset(kHSGPIO4_RST_SHIFT_RSTn);

  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();

  APP_InitAppDebugConsole();
  /* attach FRG0 clock to FLEXCOMM0 */
  auto bt_uart_cfg = BOARD_BT_UART_FRG_CLK;
  CLOCK_SetFRGClock(&bt_uart_cfg);
  CLOCK_AttachClk(BOARD_BT_UART_CLK_ATTACH);

  BOARD_SetFlexspiClock(FLEXSPI0, 2U, 4U);

  pw_sys_io_mcuxpresso_Init();
}

PW_EXTERN_C int main() {
  pw::system::Init();
  vTaskStartScheduler();
  return 0;
}

PW_NO_RETURN void pw_boot_PostMain() {
  // In case main() returns, just sit here until the device is reset.
  while (true) {
  }
  PW_UNREACHABLE;
}
