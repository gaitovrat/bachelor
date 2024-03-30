#include "board.h"
#include "clock_config.h"
#include "peripherals.h"
#include "pin_mux.h"

#include "fsl_sysmpu.h"

#include "Core.h"
#include "I2C.h"

MCU::Core core;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    BOARD_InitENETPins();
    MCU::I2C::BOARD_I2C_ReleaseBus();
    BOARD_InitACCEL_I2CPins();
    BOARD_InitGYRO_I2CPins();

    // Disable SYSMPU
    SYSMPU_Enable(SYSMPU, false);
    // Set RMII clock src
    SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

    core.init();

    while (1) {
        core.drive();
    }

    return 0;
}
