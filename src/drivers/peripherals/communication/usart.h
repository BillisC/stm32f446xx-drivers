/** @file usart.h
 *  @brief Function prototypes for the USART interface.
 *
 *  This file contains all of the enums, macros, and
 *  function prototypes required for a functional usart
 *  driver.
 *
 *  @author Vasileios Ch. (BillisC)
 *  @bug None, yet.
 */

#ifndef USART_H
#define USART_H

/* -- Includes -- */
#include <stdint.h>
#include "stm32f4xx.h"
#include "defines.h"

/* -- Structs -- */
/**
 *  @brief Contains USART registers
 */
struct __attribute__((packed)) USARTRegs {
  REG32 SR;
  REG32 DR;
  REG32 BRR;
  REG32 CR1;
  REG32 CR2;
  REG32 CR3;
  REG32 GTPR;
};

_Static_assert((sizeof(struct USARTRegs)) == (sizeof(uint32_t) * 7U),
               "USART register struct size mismatch. Is it aligned?");

#define USART(x) (struct USARTRegs *)(x)

/**
 *  @brief Contains USART interrupt configuration
 */
struct __attribute__((packed)) USARTISR {
  volatile _Bool TXEI  : 1;
  volatile _Bool CTSI  : 1;
  volatile _Bool TCI   : 1;
  volatile _Bool RXNEI : 1;
  volatile _Bool IDLEI : 1;
  volatile _Bool PEI   : 1;
  volatile _Bool LBDI  : 1;
  volatile _Bool EI    : 1;
};

_Static_assert((sizeof(struct USARTISR)) == (sizeof(uint8_t) * 1U),
               "USART interrupt struct size mismatch. Is it aligned?");

/* -- Enums -- */
/**
 *  @brief Available USART peripherals
 */
typedef enum usart_peripheral {
#ifdef USART1_BASE
  USART_PERIPH_1,
#endif
#ifdef USART2_BASE
  USART_PERIPH_2,
#endif
#ifdef USART3_BASE
  USART_PERIPH_3,
#endif
#ifdef USART6_BASE
  USART_PERIPH_6,
#endif
#ifdef UART4_BASE
  UART_PERIPH_4,
#endif
#ifdef UART5_BASE
  UART_PERIPH_5
#endif
} usart_peripheral_t;

/**
 *  @brief Available USART modes
 */
typedef enum usart_mode {
  USART_MODE_TX = 0x00,
  USART_MODE_RX = 0x01,
  USART_MODE_BO = 0x02
} usart_mode_t;

/**
 *  @brief Available USART databit number
 */
typedef enum usart_databits {
  USART_DATABITS_DB8 = 0x00,
  USART_DATABITS_DB9 = 0x01
} usart_databits_t;

/**
 *  @brief Available USART stopbit number
 */
typedef enum usart_stopbits {
  USART_STOPBITS_SB1 = 0x00,
  USART_STOPBITS_SBH = 0x01,
  USART_STOPBITS_SB2 = 0x02,
  USART_STOPBITS_SBO = 0x03
} usart_stopbits_t;

/**
 *  @brief Available USART parities
 */
typedef enum usart_parity {
  USART_PARITY_EVN = 0x00,
  USART_PARITY_ODD = 0x01,
  USART_PARITY_OFF = 0x02
} usart_parity_t;

/**
 * @brief Initiates the USART peripheral with specified options.
 *
 * The available modes for the USART peripheral are specified in
 * the usart_mode_t enum. Any other value will be ignored.
 *
 * @param usart The selected USART
 * @param baudrate The desired communication bitrate
 * @param mode The desired communication mode
 * @param x8_oversample Use x8 oversampling instead of x16
 * @return None
 */
void usart_start(const usart_peripheral_t usart, const uint32_t baudrate,
                 const usart_mode_t mode);

/**
 * @brief Configures the USART DMA functionality.
 *
 * @param usart The selected USART
 * @param forTX Enable / Disable DMA for TX
 * @param forTX Enable / Disable DMA for RX
 * @return None
 */
void usart_set_dma(const usart_peripheral_t usart, const _Bool forTX,
                   const _Bool forRX);

/**
 * @brief Enables the specified USART interrupts.
 *
 * The available interrupts are located in the usart_isr
 * struct.
 *
 * @param usart The selected USART
 * @param config The ISR configuration
 * @return None
 */
void usart_set_interrupts(const usart_peripheral_t usart,
                          const struct USARTISR config);

/**
 * @brief Sets the USART databits to the specified values.
 *
 * The available number of stopbits and databits for the USART
 * are specified in usart_stopbits_t and usart_databits_t enums
 * respectively. Any other value will be ignored.
 *
 * @param usart The selected USART
 * @param stopbits The number of stop bits
 * @param databits The word length
 * @return None
 */
void usart_set_databits(const usart_peripheral_t usart,
                        const usart_stopbits_t stopbits,
                        const usart_databits_t databits);

/**
 * @brief Sets the USART parity to the specified mode.
 *
 * The available parity modes for the USART are specified in
 * usart_parity_t. Any other value will be ignored. Please
 * note that setting the parity modes will automatically set
 * the PCE (parity control) bit. If the mode is "off" it will
 * be cleared.
 *
 * @param usart The selected USART
 * @param parity The selected parity
 * @return None
 */
void usart_set_parity(const usart_peripheral_t usart,
                      const usart_parity_t parity);

/**
 * @brief Writes specified message to USART buffer.
 *
 * In transmit/transceive mode the data will be written to the
 * DR register and be shifted out of the TX pin when ready.
 * Using it without DMA is not recommended for multibyte messages.
 *
 * @param usart The selected USART
 * @param character Pointer to the message
 * @return None
 */
void usart_tx_message(const usart_peripheral_t usart, const char *message);

/**
 * @brief Reads the received data from the USART buffer.
 *
 * In receive/transceive mode the data will be read from the
 * DR register and the RXNE flag will be cleared. Beware that
 * this method is very slow and may cause overrun errors if not
 * used in conjuction with interrupts.
 *
 * @param usart The selected USART
 * @return The received data
 */
uint16_t usart_rx_byte(const usart_peripheral_t usart);

/**
 * @brief Disable the USART interface.
 *
 * The function will wait until all transmissions are complete
 * and turn off the specified USART peripheral.
 *
 * @param usart The selected USART
 * @return None
 */
void usart_stop(const usart_peripheral_t usart);

#endif
