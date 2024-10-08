/** @file bxcan.h
 *  @brief Function prototypes for the bxCAN interface.
 *
 *  This file contains all of the enums, macros, and
 *  function prototypes required for a functional CAN
 *  interface.
 *
 *  @author Vasileios Ch. (BillisC)
 *  @bug None, yet.
 */

#ifndef BXCAN_H
#define BXCAN_H

/* -- Includes -- */
#include <stdint.h>
#include "stm32f4xx.h"
#include "defines.h"

/* -- Structs -- */
/**
 *  @brief Contains bxCAN Mailbox registers
 */
struct __attribute__((packed)) bxCANMailboxRegs {
  REG32 IR; /**< Identifier */
  REG32 TR; /**< Timestamp and length */
  REG32 LR; /**< Data low */
  REG32 HR; /**< Data high */
};

_Static_assert((sizeof(struct bxCANMailboxRegs)) == (sizeof(uint32_t) * 4U),
               "bxCAN Mailbox register struct size mismatch. Is it aligned?");

/**
 *  @brief Contains bxCAN Filter registers
 */
struct __attribute__((packed)) bxCANFilterRegs {
  REG32 FR1;
  REG32 FR2;
};

_Static_assert((sizeof(struct bxCANFilterRegs)) == (sizeof(uint32_t) * 2U),
               "bxCAN Filter register struct size mismatch. Is it aligned?");

/**
 *  @brief Contains bxCAN registers
 */
struct __attribute__((packed)) bxCANRegs {
  REG32 MCR;
  REG32 MSR;
  REG32 TSR;
  REG32 RFR[2];
  REG32 IER;
  REG32 ESR;
  REG32 BTR;
  REG32 _reserved1[88];
  struct bxCANMailboxRegs TxMailbox[3];
  struct bxCANMailboxRegs FIFOMailbox[2];
  REG32 _reserved2[12];
  REG32 FMR;
  REG32 FM1R;
  REG32 _reserved3;
  REG32 FS1R;
  REG32 _reserved4;
  REG32 FFA1R;
  REG32 _reserved5;
  REG32 FA1R;
  REG32 _reserved6[8];
  struct bxCANFilterRegs FilterBank[28];
};

_Static_assert((sizeof(struct bxCANRegs)) == (sizeof(uint32_t) * 200U),
               "bxCAN register struct size mismatch. Is it aligned?");

#define CAN(NUM) (struct bxCANRegs *)(CAN1_BASE + (0x400UL * (uint8_t)NUM))

struct __attribute__((packed)) bxCANISR {
  _Bool TME  : 1; /**< Transmit mailbox empty */
  _Bool FMP0 : 1; /**< FIFO0 pending message */
  _Bool FF0  : 1; /**< FIFO0 full */
  _Bool FOV0 : 1; /**< FIFO0 overrun */
  _Bool FMP1 : 1; /**< FIFO1 pending message */
  _Bool FF1  : 1; /**< FIFO1 full */
  _Bool FOV1 : 1; /**< FIFO1 overrun */
  _Bool EWG  : 1; /**< Error warning issued */
  _Bool EPV  : 1; /**< Error passive state */
  _Bool BOF  : 1; /**< Bus-off state */
  _Bool LEC  : 1; /**< Last error code set */
  _Bool ERR  : 1; /**< Error detected */
  _Bool WKU  : 1; /**< Wakeup */
  _Bool SLK  : 1; /**< Sleep */
};

_Static_assert((sizeof(struct bxCANISR)) == (sizeof(uint8_t) * 2U),
               "bxCAN ISR struct size mismatch. Is it aligned?");

/**
 *  @brief Contains bxCAN baudrate configuration
 */
struct __attribute__((packed)) bxCANBitrateConfig {
  uint8_t TimeSeg1       : 4;  /**< tBS1 = tq x (TimeSeg1 + 1) */
  uint8_t TimeSeg2       : 3;  /**< tBS2 = tq x (TimeSeg2 + 1) */
  uint16_t BaudPrescaler : 10; /**< tq = (BaudPrescaler + 1) x tPCLK */
};

_Static_assert((sizeof(struct bxCANBitrateConfig)) == (sizeof(uint8_t) * 3U),
               "bxCAN Bitrate config struct size mismatch. Is it aligned?");

/**
 *  @brief Contains bxCAN filter configuration
 */
struct __attribute__((packed)) bxCANFilterConfig {
  uint16_t FilterID[2];
  uint16_t FilterIDMask[2];
  _Bool ListMode    : 1;
  _Bool SingleScale : 1;
  _Bool FIFO        : 1;
  _Bool Activate    : 1;
};

_Static_assert((sizeof(struct bxCANFilterConfig)) == (sizeof(uint8_t) * 9U),
               "bxCAN Filter config struct size mismatch. Is it aligned?");

struct __attribute__((packed)) bxCANAutomationConfig {
  _Bool AutoBusOff : 1;
  _Bool AutoWakeUp : 1;
  _Bool AutoReTx   : 1;
};

/**
 *  @brief Contains bxCAN frame structure
 */
struct __attribute__((packed)) bxCANFrame {
  /* --- Header --- */
  volatile uint32_t ID;
  volatile _Bool IDE;
  volatile _Bool RTR;
  volatile uint8_t DLC;
  /* --- Time --- */
  volatile uint16_t TIME;
  volatile _Bool TGT;
  /* --- Data --- */
  volatile uint8_t DATA[8];
  // CRC handled by HW
  // ACK handled by HW
};

_Static_assert((sizeof(struct bxCANFrame)) == (sizeof(uint8_t) * 18U),
               "bxCAN Frame struct size mismatch. Is it aligned?");

/**
 *  @brief Contains bxCAN error info
 */
struct __attribute__((packed)) bxCANErrorInfo {
  uint8_t RxErrors;
  uint8_t TxErrors;
  uint8_t LastCode;
  _Bool BusOff;
  _Bool Passive;
  _Bool Warning;
};

_Static_assert((sizeof(struct bxCANErrorInfo)) == (sizeof(uint8_t) * 6U),
               "bxCAN Error info struct size mismatch. Is it aligned?");

/* -- Enums -- */
/**
 *  @brief Available bxCAN operation modes
 */
typedef enum bxcan_peripheral {
#ifdef CAN1_BASE
  BXCAN_PERIPH_1 = 0x0,
#endif
#ifdef CAN2_BASE
  BXCAN_PERIPH_2 = 0x1,
#endif
  BXCAN_PERIPH_LEN
} bxcan_peripheral_t;

/**
 *  @brief Available bxCAN operation modes
 */
typedef enum bxcan_mode {
  BXCAN_MODE_INIT = 0x0,
  BXCAN_MODE_NORM,
  BXCAN_MODE_LPOW,
} bxcan_mode_t;

/**
 *  @brief Available bxCAN test modes
 */
typedef enum bxcan_test_mode {
  BXCAN_TEST_MODE_NONE = 0x0,
  BXCAN_TEST_MODE_LOOP,
  BXCAN_TEST_MODE_SLNT,
  BXCAN_TEST_MODE_BOTH
} bxcan_test_mode_t;

/**
 *  @brief Available bxCAN FIFO priorities
 */
typedef enum bxcan_fifo_priority {
  BXCAN_FIFO_PRIORITY_ID = 0x0,
  BXCAN_FIFO_PRIORITY_RQ,
} bxcan_fifo_priority_t;

/**
 * @brief Sets the bxCAN to the specified mode
 *
 * The available operation modes for CAN are specified
 * in the bxcan_mode_t enum. Any other value will be
 * ignored.
 *
 * @param can The selected CAN
 * @param mode The selected mode
 * @return None
 */
void bxcan_set_current_mode(const bxcan_peripheral_t can,
                            const bxcan_mode_t mode);

/**
 * @brief Configures bxCAN test mode features
 *
 * The available test modes for CAN are specified
 * in the bxcan_test_mode_t enum. Any other value
 * will be ignored.
 *
 * @param can The selected CAN
 * @param mode The selected test mode
 * @return None
 */
void bxcan_set_test_mode(const bxcan_peripheral_t can,
                         const bxcan_test_mode_t mode);

/**
 * @brief Sets bxCAN time triggered communication
 *
 * @param can The selected CAN
 * @param state On/off switch
 * @return None
 */
void bxcan_set_time_triggered_mode(const bxcan_peripheral_t can,
                                   const _Bool state);

/**
 * @brief Enables the specified bxCAN interrupts
 *
 * The available interrupts are located in the bxCANISR
 * struct.
 *
 * @param can The selected CAN
 * @param config The ISR config
 * @return None
 */
void bxcan_set_interrupts(const bxcan_peripheral_t can,
                          const struct bxCANISR config);

/**
 * @brief Configures the bxCAN bitrate according to config
 *
 * The configuration options for the baudrate is
 * located in the bxCANBitrateConfig struct. Make sure
 * to not exceed the bit limit of the values and to
 * calculate the expected outcome beforehand.
 *
 * @param can The selected CAN
 * @param config The baudrate configuration
 * @return None
 */
void bxcan_configure_bitrate(const bxcan_peripheral_t can,
                             const struct bxCANBitrateConfig config);

/**
 * @brief Configures the bxCAN automation according to config
 *
 * The configuration options for the auto actions are
 * located in the bxCANAutomationConfig struct. Please
 * note that the AutoReTx is inverted when written in
 * the register.
 *
 * @param can The selected CAN
 * @param config The action configuration
 * @return None
 */
void bxcan_configure_automation(const bxcan_peripheral_t can,
                                const struct bxCANAutomationConfig config);

/**
 * @brief Configures the bxCAN FIFO according to config
 *
 * The available priorities for the FIFOs are
 * available in the bxcan_fifo_priority_t enum.
 *
 * @param can The selected CAN
 * @param lock Whether the FIFO locks on overrun
 * @param priority The FIFO priority
 * @return None
 */
void bxcan_configure_fifo(const bxcan_peripheral_t can, const _Bool lock,
                          const bxcan_fifo_priority_t priority);

/**
 * @brief Sets the desired bxCAN filter start bank
 *
 * If the MCU has a single CAN this function does
 * nothing. Otherwise a 6-bit filter number may be
 * set to change the amount of filters allowed for
 * CAN1 and CAN2 peripherals. Consult the reference
 * manual to set this appropriately. Any value above
 * six bits will be ignored.
 *
 * @param can The selected CAN
 * @param filters The CAN start bank
 * @return None
 */
void bxcan_set_filter_start(const bxcan_peripheral_t can,
                            const uint8_t filters);

/**
 * @brief Configures the bxCAN filter according to config
 *
 * The configuration options for the filter banks are
 * located in the bxCANFilterConfig struct. The ID and
 * Mask ID fields are represented by two 16-bit uints
 * each, with the 0 index being the lower nibble. The
 * filter initialization mode will be termporarily
 * enabled to apply the requested changes.
 *
 * @param can The selected CAN
 * @param filter The selected filter (0..27)
 * @param config The filter configuration
 * @return None
 */
void bxcan_configure_filter(const bxcan_peripheral_t can, const uint8_t filter,
                            const struct bxCANFilterConfig config);

/**
 *  @brief Transmits the specified CAN frame
 *
 *  The frame is put in one of the transmit mailboxes
 *  and gets sent afterwards in the bus. The frame
 *  configuration can be found under the bxCANFrame
 *  struct.
 *
 *  @param can The selected CAN
 *  @param frame Pointer to the bxCAN frame
 *  @return None
 */
void bxcan_tx_frame(const bxcan_peripheral_t can, struct bxCANFrame *frame);

/**
 *  @brief Fetches received CAN frame data
 *
 *  The data of the frame are fetched in the struct
 *  as soon as one of the FIFOs have a pending message.
 *  It is recommended to pair this with interrupts,
 *  otherwise it might be too slow. The frame configuration
 *  can be found under the bxCANFrame struct.
 *
 *  @param can The selected CAN
 *  @param FIFO The selected FIFO (Bool because it limits to 0..1)
 *  @param buffer Pointer to the Mailbox buffer
 *  @return None
 */
void bxcan_rx_frame_fetch(const bxcan_peripheral_t can, const _Bool FIFO,
                          struct bxCANMailboxRegs *buffer);

/**
 *  @brief Extracts received CAN frame data
 *
 *  The data of the register buffer are extracted
 *  in the frame struct. The frame configuration
 *  can be found under the bxCANFrame struct.
 *
 *  @param buffer The Mailbox buffer
 *  @param frame Pointer to the frame structure
 *  @return None
 */
void bxcan_rx_frame_process(const struct bxCANMailboxRegs buffer,
                            struct bxCANFrame *frame);

/**
 *  @brief Fetches all CAN errors
 *
 *  The error data are stored in a bxCANErrorInfo struct.
 *
 *  @param can The selected CAN
 *  @return Error information data
 */
const struct bxCANErrorInfo bxcan_get_error_info(const bxcan_peripheral_t can);

#endif
