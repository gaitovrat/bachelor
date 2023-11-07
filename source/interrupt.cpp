/*
 * interrupt.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: ratmir
 */
#include <lwip/arch.h>

#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
    time_isr();
}
#ifdef __cplusplus
}
#endif
