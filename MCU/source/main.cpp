#include "board.h"
#include "clock_config.h"
#include "peripherals.h"
#include "pin_mux.h"

#include "Core.h"

MCU::Core core;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    core.init();

    while (1) {
        core.drive();
    }

    return 0;
}
