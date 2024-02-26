
#ifndef TFC_CONFIG_H_
#define TFC_CONFIG_H_

#include "fsl_gpio.h"

#ifndef HW_TFC_PCB_VERSION
#define HW_TFC_PCB_VERSION 20
#endif

static inline void GPIO_SetPinsOutput(GPIO_Type *base, uint32_t mask) {
    GPIO_PortSet(base, mask);
}

static inline void GPIO_ClearPinsOutput(GPIO_Type *base, uint32_t mask) {
    GPIO_PortClear(base, mask);
}

#endif
