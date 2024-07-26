#include "mcu_init.h"

// Common
#include "../common/defines.h"

// Drivers
#include "stm32f4xx.h"

#include <stdint.h>

static void clock_init(void) {
  /* Enable 8 MHz HSE oscillator (Source: STLINK) */
  RCC->CR |= RCC_CR_HSEBYP_Msk | RCC_CR_HSEON_Msk;
  while (! (RCC->CR & RCC_CR_HSERDY_Msk)) { ASM_NOP; };

  /* Enable power controller and change voltage
   * regulator scaling to 1. */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN_Msk;
  /* Do some dummy reads */
  volatile uint32_t dummy_read;
  dummy_read = RCC->APB1ENR;
  dummy_read = RCC->APB1ENR;

  PWR->CR |= (0b11 << PWR_CR_VOS_Pos);

  /* Configure flash controller for 3V3 and 180 MHz
   * system clock (5 wait states). */
  FLASH->ACR |= FLASH_ACR_LATENCY_5WS;

  /* Clear PLL clock parameters */
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk |
                    RCC_PLLCFGR_PLLN_Msk |
                    RCC_PLLCFGR_PLLP_Msk);

  /* Configure the PLL clock */
  RCC->PLLCFGR |= ((PLLM_VAL << RCC_PLLCFGR_PLLM_Pos) |
                   (PLLN_VAL << RCC_PLLCFGR_PLLN_Pos) |
                   (PLLSRC_VAL << RCC_PLLCFGR_PLLSRC_Pos));

  /* Enable PLL clock */
  RCC->CR |= RCC_CR_PLLON_Msk;
  while (! (RCC->CR & RCC_CR_PLLRDY_Msk)) { ASM_NOP; };

  #if EN_OVERDRIVE == 1
  /* Enable overdrive mode */
  PWR->CR |= (PWR_CR_ODEN_Msk);
  while (! (PWR->CSR & PWR_CSR_ODRDY)) { ASM_NOP; };

  /* Switch internal voltage regulator to
   * overdrive mode. */
  PWR->CR |= (PWR_CR_ODSWEN_Msk);
  while (! (PWR->CSR & PWR_CSR_ODSWRDY)) { ASM_NOP; };
  #endif

  /* Select PLL as the system clock source */
  RCC->CFGR |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
  while (! (RCC->CFGR & RCC_CFGR_SWS_PLL)) { ASM_NOP; };

  /* Inform CMSIS about the system clock */
  SystemCoreClockUpdate();
}

void mcu_init(void) {
  clock_init();
  /* Set interrupt rate to 1 KHz (/180 MHZ) and enable interrupts */
  SysTick_Config(180000);
  __enable_irq();
}