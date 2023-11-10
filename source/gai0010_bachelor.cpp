#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
#ifdef USE_ENET
#include "Enet.h"
Enet enet;
#endif

#define PRINTLN(X) (PRINTF(X "\r\n"))

int main(void) {
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    // Init debug console
    BOARD_InitDebugConsole();
#endif
#ifdef USE_ENET
    enet.Init(1024, 8080);
#endif

    while(1) {
        PRINTLN("Hello world");
    }

    return 0 ;
}
