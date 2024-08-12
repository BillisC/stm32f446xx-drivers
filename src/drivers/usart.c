/** @file usart.c
 *  @brief Function defines for the USART driver.
 *
 *  This file contains all of the function definitions
 *  declared in usart.h.
 *
 *  @author Vasileios Ch. (BillisC)
 *  @bug None, yet.
 */

/* -- Includes -- */
#include "defines.h"
#include "usart.h"

void usart_start(const usart_sel_t usart, const uint32_t baudrate,
                 const usart_mode_t mode) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  }

  struct USARTRegs *regs = USART(usart);

  /* Calculate USART div accurately */
  regs->BRR = ((APB1_CLK * 1000000) / baudrate);

  /* Setup communication modes */
  volatile uint32_t cr1 = regs->CR1;
  cr1 &= ~(USART_CR1_TE_Msk | USART_CR1_RE_Msk); // Clear first
  if (mode == USART_MODE_TX) {
    cr1 |= USART_CR1_TE_Msk;
  } else if (mode == USART_MODE_RX) {
    cr1 |= USART_CR1_RE_Msk;
  } else {
    cr1 |= (USART_CR1_TE_Msk | USART_CR1_RE_Msk);
  }

  /* Power on USART interface */
  regs->CR1 = (cr1 | USART_CR1_UE_Msk);
}

void usart_set_dma(const usart_sel_t usart, const _Bool forTX,
                   const _Bool forRX) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  };

  struct USARTRegs *regs = USART(usart);

  /* Configure DMA parameters */
  volatile uint32_t cr3 = regs->CR3;
  cr3 &= ~(USART_CR3_DMAT_Msk | USART_CR3_DMAR_Msk); // Clear first
  cr3 |= (((1UL & forTX) << USART_CR3_DMAT_Pos) |
          ((1UL & forRX) << USART_CR3_DMAR_Pos));

  regs->CR3 = cr3;
}

void usart_set_interrupts(const usart_sel_t usart,
                          const struct USARTISR config) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  };

  struct USARTRegs *regs = USART(usart);

  /* Set USART interrupts */
  volatile uint8_t cr1 = regs->CR1;
  cr1 &= ~(USART_CR1_PEIE_Msk | USART_CR1_TXEIE_Msk | USART_CR1_TCIE_Msk |
           USART_CR1_RXNEIE_Msk | USART_CR1_IDLEIE_Msk); // Clear first
  cr1 |= ((config.PEI << USART_CR1_PEIE_Pos) |
          (config.TXEI << USART_CR1_TXEIE_Pos) |
          (config.TCI << USART_CR1_TCIE_Pos) |
          (config.RXNEI << USART_CR1_RXNEIE_Pos) |
          (config.IDLEI << USART_CR1_IDLEIE_Pos));

  regs->CR1 = cr1;

  volatile uint8_t cr2 = regs->CR2;
  cr2 &= ~(USART_CR2_LBDIE_Msk); // Clear first
  cr2 |= (config.LBDI << USART_CR2_LBDIE_Pos);

  regs->CR2 = cr2;

  volatile uint8_t cr3 = regs->CR3;
  cr3 &= ~(USART_CR3_CTSIE_Msk | USART_CR3_EIE_Msk); // Clear first
  cr3 |= ((config.CTSI << USART_CR3_CTSIE_Pos) |
          (config.EI << USART_CR3_EIE_Pos)); //

  regs->CR3 = cr3;
}

void usart_set_databits(const usart_sel_t usart,
                        const usart_stopbits_t stopbits,
                        const usart_databits_t databits) {
  /* Make sure the stopbits are valid */
  switch (stopbits) {
    case USART_STOPBITS_SB1:
    case USART_STOPBITS_SBH:
    case USART_STOPBITS_SB2:
    case USART_STOPBITS_SBO: break;

    default: return;
  }

  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  }

  /* Make sure the number of databits is valid */
  if ((databits != USART_DATABITS_DB8) && (databits != USART_DATABITS_DB9)) {
    return;
  } else {
    struct USARTRegs *regs = USART(usart);

    /* Set amount of databits */
    volatile uint32_t cr1 = regs->CR1;
    cr1 &= ~(USART_CR1_M_Msk); // Clear first
    cr1 |= (databits << USART_CR1_M_Pos);

    regs->CR1 = cr1;

    /* Set amount of stopbits */
    volatile uint32_t cr2 = regs->CR2;
    cr2 &= ~(USART_CR2_STOP_Msk); // Clear first
    cr2 |= (stopbits << USART_CR2_STOP_Pos);

    regs->CR2 = cr2;
  }
}

void usart_set_parity(const usart_sel_t usart, const usart_parity_t parity) {
  /* Check that the parity is valid */
  switch (parity) {
    case USART_PARITY_EVN:
    case USART_PARITY_ODD:
    case USART_PARITY_OFF: break;

    default: return;
  }

  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  }

  struct USARTRegs *regs = USART(usart);

  /* Set amount of databits */
  volatile uint32_t cr1 = regs->CR1;

  if (parity == USART_PARITY_EVN) {
    cr1 &= ~(USART_CR1_PS_Msk);
    cr1 |= USART_CR1_PCE_Msk;
  } else if (parity == USART_PARITY_ODD) {
    cr1 |= (USART_CR1_PS_Msk | USART_CR1_PCE_Msk);
  } else {
    cr1 &= ~(USART_CR1_PCE_Msk);
  }

  regs->CR1 = cr1;
}

inline static void usart_tx_byte(struct USARTRegs *regs, const char character) {
  /* Wait for TXE and transmit data */
  while (!(regs->SR & USART_SR_TXE_Msk)) { ASM_NOP; };
  regs->DR = character;
}

void usart_tx_message(const usart_sel_t usart, const char *message) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  }

  struct USARTRegs *regs = USART(usart);

  /* Send message */
  while (*message != '\0') { usart_tx_byte(regs, *message++); }

  /* Wait for transmission complete flag  */
  while (!(regs->SR & USART_SR_TC_Msk)) { ASM_NOP; };
}

uint16_t usart_rx_byte(const usart_sel_t usart) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return '\0';
  }

  struct USARTRegs *regs = USART(usart);

  /* Read received data */
  while (!(regs->SR & USART_SR_RXNE_Msk)) { ASM_NOP; };
  const uint16_t word = regs->DR;

  return word;
}

void usart_stop(const usart_sel_t usart) {
  /* Check that the USART exists */
  switch (usart) {
    case USART_SEL_1:
    case USART_SEL_2:
    case USART_SEL_3:
    case UART_SEL_4:
    case UART_SEL_5:
    case USART_SEL_6: break;

    default: return;
  }

  struct USARTRegs *regs = USART(usart);

  /* Wait for all transmissions to end */
  while (!(regs->SR & USART_SR_TC_Msk)) { ASM_NOP; };
  regs->CR1 &= ~(USART_CR1_UE_Msk);
}
