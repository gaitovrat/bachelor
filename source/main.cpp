#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
#include "assert.h"

#include "sensors/FXOS8700CQ.h"
#ifdef USE_ENET
#include "Enet.h"
Enet enet;
#endif

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

    FXOS8700CQ accel(FXOS8700CQ::Range::G_4);
    accel.Init();

    while(1) {
    	FXOS8700CQ::Data data = accel.Read();
		PRINTF("Accel: %5d %5d %5d\r\n", data.Accel.X, data.Accel.Y, data.Accel.Z);
		PRINTF("Mag: %5d %5d %5d\r\n", data.Mag.X, data.Mag.Y, data.Mag.Z);
    }

    return 0 ;
}
