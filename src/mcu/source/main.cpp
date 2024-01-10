#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
#include "assert.h"

#include "sensors/FXOS8700CQ.h"
#include "sensors/FXAS21002.h"
#ifdef USE_ENET
#include "Enet.h"
Enet enet;
#endif
FXOS8700CQ accel(FXOS8700CQ::Range::G_4);
FXAS21002 gyro(FXAS21002::Range::DPS_1000);

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
    enet.init(1024, 8080);
#endif

    accel.init();
    gyro.init();

    while(1) {
    	std::optional<FXOS8700CQ::Data> accelData = accel.read();
    	std::optional<Vec3> gyroData = gyro.read();

    	if (accelData.has_value()) {
			PRINTF("Accel: %5d %5d %5d\r\n", accelData->accel.x, accelData->accel.y, accelData->accel.z);
			PRINTF("Mag: %5d %5d %5d\r\n", accelData->mag.x, accelData->mag.y, accelData->mag.z);
    	}

    	if (gyroData.has_value()) {
    		PRINTF("Gyro: %5d %5d %5d\r\n", gyroData->x, gyroData->y, gyroData->z);
    	}
    }

    return 0 ;
}
