/* ------------------
 * MCU INITIALIZATION
 * ------------------ */

#ifndef MCU_INIT_H
#define MCU_INIT_H

/* Clock Configuration
 * 8MHz (HSE) / 4 (PLLM) = 2MHz
 * 2MHz * 180 (PLLN) = 360MHz
 * 360MHz / 2 (PLLSRC) = 180MHz */
#define PLLM_VAL 4
#define PLLN_VAL 180
#define PLLSRC_VAL 1

/* Power Regulation */
#define EN_OVERDRIVE 1

/**
 * MCU initialization function.
*/
void mcu_init(void);

#endif