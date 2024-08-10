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

/* -- Structs -- */
struct __attribute__((packed)) usart {
  volatile uint32_t SR;
  volatile uint32_t DR;
  volatile uint32_t BRR;
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t GTPR;
};

_Static_assert((sizeof(struct usart)) == (sizeof(uint32_t) * 7U),
               "USART register struct size mismatch. Is it aligned?");

struct __attribute__((packed)) usart_isr {
  volatile _Bool TXEI  : 1;
  volatile _Bool CTSI  : 1;
  volatile _Bool TCI   : 1;
  volatile _Bool RXNEI : 1;
  volatile _Bool IDLEI : 1;
  volatile _Bool PEI   : 1;
  volatile _Bool LBDI  : 1;
  volatile _Bool EI    : 1;
};

_Static_assert((sizeof(struct usart_isr)) == (sizeof(uint8_t) * 1U),
               "USART interrupt struct size mismatch. Is it aligned?");

/* -- Enums -- */
typedef enum usart_sel {
  usart1 = USART1_BASE,
  usart2 = USART2_BASE,
  usart3 = USART3_BASE,
  usart6 = USART6_BASE,
  uart4 = UART4_BASE,
  uart5 = UART5_BASE,
} usart_sel_t;

#define USART(sel) (struct usart *)((usart_sel_t)sel)

typedef enum usart_mode {
  tx = 0x00,
  rx = 0x01,
  bo = 0x02
} usart_mode_t;

typedef enum usart_databits {
  db8 = 0x00,
  db9 = 0x01
} usart_databits_t;

typedef enum usart_stopbits {
  sb1 = 0x00,
  sbh = 0x01,
  sb2 = 0x02,
  sbo = 0x03
} usart_stopbits_t;

typedef enum usart_parity {
  evn = 0x00,
  odd = 0x01,
  off = 0x02
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
void usart_start(const usart_sel_t usart, const uint32_t baudrate,
                 const usart_mode_t mode);

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
void usart_set_interrupts(const usart_sel_t usart,
                          const struct usart_isr config);

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
void usart_set_databits(const usart_sel_t usart,
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
void usart_set_parity(const usart_sel_t usart, const usart_parity_t parity);

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
void usart_tx_message(const usart_sel_t usart, const char *message);

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
uint16_t usart_rx_byte(const usart_sel_t usart);

/**
 * @brief Disable the USART interface.
 *
 * The function will wait until all transmissions are complete
 * and turn off the specified USART peripheral.
 *
 * @param usart The selected USART
 * @return None
 */
void usart_stop(const usart_sel_t usart);

#endif
