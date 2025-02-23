/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLUP_EN 0x20u        /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC0 0x00u            /*!<@brief Selects pin function 0 */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name PIO0_10 (coord K16), USER_KEY2
  @{ */
/* Routed pin properties */
#define BOARD_INITBUTTONPINS_SW2_PERIPHERAL GPIO           /*!<@brief Peripheral name */
#define BOARD_INITBUTTONPINS_SW2_SIGNAL PIO0               /*!<@brief Signal name */
#define BOARD_INITBUTTONPINS_SW2_CHANNEL 10                /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITBUTTONPINS_SW2_GPIO GPIO                 /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITBUTTONPINS_SW2_GPIO_PIN_MASK (1U << 10U) /*!<@brief GPIO pin mask */
#define BOARD_INITBUTTONPINS_SW2_PORT 0U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITBUTTONPINS_SW2_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITBUTTONPINS_SW2_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PIO0_25 (coord C12), USER_KEY1
  @{ */
/* Routed pin properties */
#define BOARD_INITBUTTONPINS_SW1_PERIPHERAL GPIO           /*!<@brief Peripheral name */
#define BOARD_INITBUTTONPINS_SW1_SIGNAL PIO0               /*!<@brief Signal name */
#define BOARD_INITBUTTONPINS_SW1_CHANNEL 25                /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITBUTTONPINS_SW1_GPIO GPIO                 /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITBUTTONPINS_SW1_GPIO_PIN_MASK (1U << 25U) /*!<@brief GPIO pin mask */
#define BOARD_INITBUTTONPINS_SW1_PORT 0U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITBUTTONPINS_SW1_PIN 25U                   /*!<@brief PORT pin number */
#define BOARD_INITBUTTONPINS_SW1_PIN_MASK (1U << 25U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitBUTTONPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u      /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u   /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u          /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u       /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u       /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u         /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u    /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u    /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PULLUP_EN 0x20u      /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_DI 0x00u        /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u        /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_SLOW 0x80u /*!<@brief Slow mode */

/*! @name I3C0_PUR (coord C14), J18[3]
  @{ */
/* Routed pin properties */
#define BOARD_INITI3CPINS_I3C0_PUR_PERIPHERAL I3C0   /*!<@brief Peripheral name */
#define BOARD_INITI3CPINS_I3C0_PUR_SIGNAL PUR        /*!<@brief Signal name */
#define BOARD_INITI3CPINS_I3C0_PUR_PORT 2U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITI3CPINS_I3C0_PUR_PIN 31U                   /*!<@brief PORT pin number */
#define BOARD_INITI3CPINS_I3C0_PUR_PIN_MASK (1U << 31U)      /*!<@brief PORT pin mask */
                                                             /* @} */

/*! @name I3C0_SDA (coord D10), J18[2]/U8[3]/U114[A2]
  @{ */
/* Routed pin properties */
#define BOARD_INITI3CPINS_I3C0_SDA_PERIPHERAL I3C0               /*!<@brief Peripheral name */
#define BOARD_INITI3CPINS_I3C0_SDA_SIGNAL SDA                    /*!<@brief Signal name */
#define BOARD_INITI3CPINS_I3C0_SDA_PORT 2U                       /*!<@brief PORT peripheral base pointer */
#define BOARD_INITI3CPINS_I3C0_SDA_PIN 30U                       /*!<@brief PORT pin number */
#define BOARD_INITI3CPINS_I3C0_SDA_PIN_MASK (1U << 30U)          /*!<@brief PORT pin mask */
                                                                 /* @} */

/*! @name I3C0_SCL (coord B10), J18[1]/U8[2]/U114[A1]
  @{ */
/* Routed pin properties */
#define BOARD_INITI3CPINS_I3C0_SCL_PERIPHERAL I3C0               /*!<@brief Peripheral name */
#define BOARD_INITI3CPINS_I3C0_SCL_SIGNAL SCL                    /*!<@brief Signal name */
#define BOARD_INITI3CPINS_I3C0_SCL_PORT 2U                       /*!<@brief PORT peripheral base pointer */
#define BOARD_INITI3CPINS_I3C0_SCL_PIN 29U                       /*!<@brief PORT pin number */
#define BOARD_INITI3CPINS_I3C0_SCL_PIN_MASK (1U << 29U)          /*!<@brief PORT pin mask */
                                                                 /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitI3CPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u      /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u   /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC0 0x00u          /*!<@brief Selects pin function 0 */
#define IOPCTL_PIO_INBUF_EN 0x40u       /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u         /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u    /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLUP_EN 0x20u      /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_EN 0x10u        /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_SLOW 0x80u /*!<@brief Slow mode */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitI3CPinsAsGPIO(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_FUNC6 0x06u            /*!<@brief Selects pin function 6 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PULLUP_EN 0x20u        /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name SWO (coord N3), U7[12]
  @{ */
/* Routed pin properties */
#define BOARD_INITDEBUG_UARTPINS_SWO_PERIPHERAL SWD /*!<@brief Peripheral name */
#define BOARD_INITDEBUG_UARTPINS_SWO_SIGNAL SWO     /*!<@brief Signal name */
#define BOARD_INITDEBUG_UARTPINS_SWO_PORT 2U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITDEBUG_UARTPINS_SWO_PIN 24U                   /*!<@brief PORT pin number */
#define BOARD_INITDEBUG_UARTPINS_SWO_PIN_MASK (1U << 24U)      /*!<@brief PORT pin mask */
                                                               /* @} */

/*! @name FC12_TXD_SCL_MISO (coord N8), J27[2]/J43[5]/FXIO_D10
  @{ */
/* Routed pin properties */
#define BOARD_INITDEBUG_UARTPINS_D10_PERIPHERAL FLEXCOMM12          /*!<@brief Peripheral name */
#define BOARD_INITDEBUG_UARTPINS_D10_SIGNAL TXD_SCL_MISO            /*!<@brief Signal name */
#define BOARD_INITDEBUG_UARTPINS_D10_PORT 4U                        /*!<@brief PORT peripheral base pointer */
#define BOARD_INITDEBUG_UARTPINS_D10_PIN 30U                        /*!<@brief PORT pin number */
#define BOARD_INITDEBUG_UARTPINS_D10_PIN_MASK (1U << 30U)           /*!<@brief PORT pin mask */
                                                                    /* @} */

/*! @name FC12_RXD_SDA_MOSI (coord N10), J27[1]/J43[8]/FXIO_D11
  @{ */
/* Routed pin properties */
#define BOARD_INITDEBUG_UARTPINS_D11_PERIPHERAL FLEXCOMM12          /*!<@brief Peripheral name */
#define BOARD_INITDEBUG_UARTPINS_D11_SIGNAL RXD_SDA_MOSI            /*!<@brief Signal name */
#define BOARD_INITDEBUG_UARTPINS_D11_PORT 4U                        /*!<@brief PORT peripheral base pointer */
#define BOARD_INITDEBUG_UARTPINS_D11_PIN 31U                        /*!<@brief PORT pin number */
#define BOARD_INITDEBUG_UARTPINS_D11_PIN_MASK (1U << 31U)           /*!<@brief PORT pin mask */
                                                                    /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UARTPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name FC0_TXD_SCL_MISO_WS (coord G16), J45[32]/U105[3]/U9[12]
  @{ */
/* Routed pin properties */
/*!
 * @brief Peripheral name */
#define BOARD_INITM2UARTPINS_DEBUG_UART_TXD_PERIPHERAL FLEXCOMM0
/*!
 * @brief Signal name */
#define BOARD_INITM2UARTPINS_DEBUG_UART_TXD_SIGNAL TXD_SCL_MISO_WS
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITM2UARTPINS_DEBUG_UART_TXD_PORT 0U
/*!
 * @brief PORT pin number */
#define BOARD_INITM2UARTPINS_DEBUG_UART_TXD_PIN 1U
/*!
 * @brief PORT pin mask */
#define BOARD_INITM2UARTPINS_DEBUG_UART_TXD_PIN_MASK (1U << 1U)
/* @} */

/*! @name FC0_RXD_SDA_MOSI_DATA (coord H16), J45[22]/U9[13]/U105[4]
  @{ */
/* Routed pin properties */
/*!
 * @brief Peripheral name */
#define BOARD_INITM2UARTPINS_DEBUG_UART_RX_PERIPHERAL FLEXCOMM0
/*!
 * @brief Signal name */
#define BOARD_INITM2UARTPINS_DEBUG_UART_RX_SIGNAL RXD_SDA_MOSI_DATA
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITM2UARTPINS_DEBUG_UART_RX_PORT 0U
/*!
 * @brief PORT pin number */
#define BOARD_INITM2UARTPINS_DEBUG_UART_RX_PIN 2U
/*!
 * @brief PORT pin mask */
#define BOARD_INITM2UARTPINS_DEBUG_UART_RX_PIN_MASK (1U << 2U)
/* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitM2UARTPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_EN 0x0100u   /*!<@brief Full drive enable */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name FLEXSPI0_DATA0 (coord T15), U38[D3]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_SIGNAL FLEXSPI_DATA0    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_SIGNAL FLEXSPI_DATA0
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_PIN 20U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA0_PIN_MASK (1U << 20U)
/* @} */

/*! @name FLEXSPI0_DATA1 (coord T14), U38[D2]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_SIGNAL FLEXSPI_DATA1    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_SIGNAL FLEXSPI_DATA1
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_PIN 21U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA1_PIN_MASK (1U << 21U)
/* @} */

/*! @name FLEXSPI0_DATA2 (coord R13), U38[C4]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_SIGNAL FLEXSPI_DATA2    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_SIGNAL FLEXSPI_DATA2
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_PIN 22U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA2_PIN_MASK (1U << 22U)
/* @} */

/*! @name FLEXSPI0_DATA3 (coord R12), U38[D4]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_SIGNAL FLEXSPI_DATA3    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_SIGNAL FLEXSPI_DATA3
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_PIN 23U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA3_PIN_MASK (1U << 23U)
/* @} */

/*! @name FLEXSPI0_DATA4 (coord N12), U38[D5]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_SIGNAL FLEXSPI_DATA4    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_SIGNAL FLEXSPI_DATA4
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_PIN 24U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA4_PIN_MASK (1U << 24U)
/* @} */

/*! @name FLEXSPI0_DATA5 (coord R14), U38[E3]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_SIGNAL FLEXSPI_DATA5    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_SIGNAL FLEXSPI_DATA5
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_PIN 25U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA5_PIN_MASK (1U << 25U)
/* @} */

/*! @name FLEXSPI0_DATA6 (coord P14), U38[E2]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_SIGNAL FLEXSPI_DATA6    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_SIGNAL FLEXSPI_DATA6
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_PIN 26U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA6_PIN_MASK (1U << 26U)
/* @} */

/*! @name FLEXSPI0_DATA7 (coord P13), U38[E1]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_SIGNAL FLEXSPI_DATA7    /*!<@brief Signal name */
                                                                       /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_SIGNAL FLEXSPI_DATA7
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_PIN 27U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DATA7_PIN_MASK (1U << 27U)
/* @} */

/*! @name FLEXSPI0_SCLK (coord T17), U38[B2]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_PERIPHERAL FLEXSPI0    /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_SIGNAL FLEXSPI_SCLK    /*!<@brief Signal name */
                                                                     /* @} */
/*!
 * @brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_SIGNAL FLEXSPI_SCLK
/*!
 * @brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_PORT 1U
/*!
 * @brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_PIN 18U
/*!
 * @brief PORT pin mask */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_SCLK_PIN_MASK (1U << 18U)
/* @} */

/*! @name FLEXSPI0_SS0_N (coord U16), U38[C2]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_CS_PERIPHERAL FLEXSPI0     /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_CS_SIGNAL FLEXSPI_SS0_N    /*!<@brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_CS_PORT 1U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_CS_PIN 19U                   /*!<@brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_CS_PIN_MASK (1U << 19U)      /*!<@brief PORT pin mask */
                                                                      /* @} */

/*! @name FLEXSPI0_DQS (coord U14), U38[C3]
  @{ */
/* Routed pin properties */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DQS_PERIPHERAL FLEXSPI0   /*!<@brief Peripheral name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DQS_SIGNAL FLEXSPI_DQS    /*!<@brief Signal name */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DQS_PORT 1U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DQS_PIN 28U                   /*!<@brief PORT pin number */
#define BOARD_INITFLEXSPI0BPINS_FLEXSPI0_DQS_PIN_MASK (1U << 28U)      /*!<@brief PORT pin mask */
                                                                       /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitFlexSPI0BPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC0 0x00u            /*!<@brief Selects pin function 0 */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_FUNC7 0x07u            /*!<@brief Selects pin function 7 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PULLUP_EN 0x20u        /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name PIO2_10 (coord N5), SD0_RESET_N
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSM2_SD0_RESET_N_PERIPHERAL GPIO           /*!<@brief Peripheral name */
#define BOARD_INITPINSM2_SD0_RESET_N_SIGNAL PIO2               /*!<@brief Signal name */
#define BOARD_INITPINSM2_SD0_RESET_N_CHANNEL 10                /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINSM2_SD0_RESET_N_GPIO GPIO                 /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINSM2_SD0_RESET_N_GPIO_PIN_MASK (1U << 10U) /*!<@brief GPIO pin mask */
#define BOARD_INITPINSM2_SD0_RESET_N_PORT 2U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSM2_SD0_RESET_N_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSM2_SD0_RESET_N_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                               /* @} */

/*! @name PIO4_3 (coord M14), M2_3V3
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSM2_M2_3V3_PERIPHERAL GPIO                    /*!<@brief Peripheral name */
#define BOARD_INITPINSM2_M2_3V3_SIGNAL PIO4                        /*!<@brief Signal name */
#define BOARD_INITPINSM2_M2_3V3_CHANNEL 3                          /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINSM2_M2_3V3_GPIO GPIO                          /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINSM2_M2_3V3_GPIO_PIN_MASK (1U << 3U)           /*!<@brief GPIO pin mask */
#define BOARD_INITPINSM2_M2_3V3_PORT 4U                            /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSM2_M2_3V3_PIN 3U                             /*!<@brief PORT pin number */
#define BOARD_INITPINSM2_M2_3V3_PIN_MASK (1U << 3U)                /*!<@brief PORT pin mask */
                                                                   /* @} */

/*! @name PIO0_31 (coord D12), WL_RST
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSM2_WL_RST_PERIPHERAL GPIO                    /*!<@brief Peripheral name */
#define BOARD_INITPINSM2_WL_RST_SIGNAL PIO0                        /*!<@brief Signal name */
#define BOARD_INITPINSM2_WL_RST_CHANNEL 31                         /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINSM2_WL_RST_GPIO GPIO                          /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINSM2_WL_RST_GPIO_PIN_MASK (1U << 31U)          /*!<@brief GPIO pin mask */
#define BOARD_INITPINSM2_WL_RST_PORT 0U                            /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSM2_WL_RST_PIN 31U                            /*!<@brief PORT pin number */
#define BOARD_INITPINSM2_WL_RST_PIN_MASK (1U << 31U)               /*!<@brief PORT pin mask */
                                                                   /* @} */

/*! @name PIO3_20 (coord D6), SDIO_RST
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSM2_SDIO_RST_PERIPHERAL GPIO                    /*!<@brief Peripheral name */
#define BOARD_INITPINSM2_SDIO_RST_SIGNAL PIO3                        /*!<@brief Signal name */
#define BOARD_INITPINSM2_SDIO_RST_CHANNEL 20                         /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINSM2_SDIO_RST_GPIO GPIO                          /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINSM2_SDIO_RST_GPIO_PIN_MASK (1U << 20U)          /*!<@brief GPIO pin mask */
#define BOARD_INITPINSM2_SDIO_RST_PORT 3U                            /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSM2_SDIO_RST_PIN 20U                            /*!<@brief PORT pin number */
#define BOARD_INITPINSM2_SDIO_RST_PIN_MASK (1U << 20U)               /*!<@brief PORT pin mask */
                                                                     /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPinsM2(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_EN 0x0100u   /*!<@brief Full drive enable */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name MCLK (coord M5), JP27[2]
  @{ */
/* Routed pin properties */
#define BOARD_INITAUDIOPINS_MCLK_PERIPHERAL CLKCTL /*!<@brief Peripheral name */
#define BOARD_INITAUDIOPINS_MCLK_SIGNAL MCLK       /*!<@brief Signal name */
#define BOARD_INITAUDIOPINS_MCLK_PORT 1U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITAUDIOPINS_MCLK_PIN 10U                   /*!<@brief PORT pin number */
#define BOARD_INITAUDIOPINS_MCLK_PIN_MASK (1U << 10U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitAudioPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitM2ScoPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC6 0x06u            /*!<@brief Selects pin function 6 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitArduinoUARTPins(void); /* Function assigned for the Cortex-M33 */

#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!<@brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!<@brief Normal drive */
#define IOPCTL_PIO_FUNC0 0x00u            /*!<@brief Selects pin function 0 */
#define IOPCTL_PIO_FUNC1 0x01u            /*!<@brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!<@brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!<@brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!<@brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!<@brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!<@brief Enable pull-down function */
#define IOPCTL_PIO_PULLUP_EN 0x20u        /*!<@brief Enable pull-up function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!<@brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!<@brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!<@brief Normal mode */

/*! @name SD0_CARD_DET_N (coord T3), J32[10]/SD0_CARD_DET_N
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_CARD_DET_N_PERIPHERAL USDHC0              /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_CARD_DET_N_SIGNAL USDHC_CARD_DET          /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_CARD_DET_N_PORT 2U                        /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_CARD_DET_N_PIN 9U                         /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_CARD_DET_N_PIN_MASK (1U << 9U)            /*!<@brief PORT pin mask */
                                                                       /* @} */

/*! @name SD0_D0 (coord U4), U111A[A3]/SD0_D0
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_D0_PERIPHERAL USDHC0        /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_D0_SIGNAL USDHC_DATA        /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_D0_CHANNEL 0                /*!<@brief Signal channel */
#define BOARD_INITPINSSD_SD0_D0_PORT 2U                  /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_D0_PIN 0U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_D0_PIN_MASK (1U << 0U)      /*!<@brief PORT pin mask */
                                                         /* @} */

/*! @name SD0_D1 (coord T4), U111A[A4]/SD0_D1
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_D1_PERIPHERAL USDHC0        /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_D1_SIGNAL USDHC_DATA        /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_D1_CHANNEL 1                /*!<@brief Signal channel */
#define BOARD_INITPINSSD_SD0_D1_PORT 2U                  /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_D1_PIN 1U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_D1_PIN_MASK (1U << 1U)      /*!<@brief PORT pin mask */
                                                         /* @} */

/*! @name SD0_D2 (coord T7), U111A[A5]/SD0_D2
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_D2_PERIPHERAL USDHC0        /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_D2_SIGNAL USDHC_DATA        /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_D2_CHANNEL 2                /*!<@brief Signal channel */
#define BOARD_INITPINSSD_SD0_D2_PORT 2U                  /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_D2_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_D2_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                                         /* @} */

/*! @name SD0_D3 (coord U6), U111A[B2]/SD0_D3
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_D3_PERIPHERAL USDHC0        /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_D3_SIGNAL USDHC_DATA        /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_D3_CHANNEL 3                /*!<@brief Signal channel */
#define BOARD_INITPINSSD_SD0_D3_PORT 2U                  /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_D3_PIN 3U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_D3_PIN_MASK (1U << 3U)      /*!<@brief PORT pin mask */
                                                         /* @} */

/*! @name SD0_CMD (coord R6), U111A[M5]/SD0_CMD
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_CMD_PERIPHERAL USDHC0         /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_CMD_SIGNAL USDHC_CMD          /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_CMD_PORT 1U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_CMD_PIN 31U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_CMD_PIN_MASK (1U << 31U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name SD0_CLK (coord R5), U111A[M6]/SD0_CLK
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SD0_CLK_PERIPHERAL USDHC0         /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SD0_CLK_SIGNAL USDHC_CLK          /*!<@brief Signal name */
#define BOARD_INITPINSSD_SD0_CLK_PORT 1U                   /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SD0_CLK_PIN 30U                   /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SD0_CLK_PIN_MASK (1U << 30U)      /*!<@brief PORT pin mask */
                                                           /* @} */

/*! @name PIO4_0 (coord N15), U117[B2]
  @{ */
/* Routed pin properties */
#define BOARD_INITPINSSD_SDC_3V3_PERIPHERAL GPIO                    /*!<@brief Peripheral name */
#define BOARD_INITPINSSD_SDC_3V3_SIGNAL PIO4                        /*!<@brief Signal name */
#define BOARD_INITPINSSD_SDC_3V3_CHANNEL 0                          /*!<@brief Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINSSD_SDC_3V3_GPIO GPIO                          /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITPINSSD_SDC_3V3_GPIO_PIN_MASK (1U << 0U)           /*!<@brief GPIO pin mask */
#define BOARD_INITPINSSD_SDC_3V3_PORT 4U                            /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINSSD_SDC_3V3_PIN 0U                             /*!<@brief PORT pin number */
#define BOARD_INITPINSSD_SDC_3V3_PIN_MASK (1U << 0U)                /*!<@brief PORT pin mask */
                                                                    /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPinsSD(void); /* Function assigned for the Cortex-M33 */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
