#include "port.h"
#include "armTimer.h"
#include "pdc8544.h"
volatile word *armTimerRegAddr = (word *)ARM_TIMER_BASE;  // base pointer for registry access
/*
Only one in continuous mode.
The clock from the ARM timer is derived from the system clock. This clock can change dynamically
e.g. if the system goes into reduced power or in low power mode. Thus the clock speed adapts to
the overal system performance capabilities. For accurate timing it is recommended to use the
system timers. 
*/
void enableARMtimer(void) {
    armTimerRegAddr[ARM_TIMER_CONTROL] = ARM_TIMER_ENABLE|ARM_TIMER_INT_ENABLE|ARM_TIMER_23BIT; // pre-scale 1 == 0x00
    armTimerRegAddr[ARM_TIMER_PREDIV] = 249; // should be default, but let's make sure
}

void setARMtimerInterval(word interval) { // Clk/256 * interval  250/256 * 1024(0x400) = 1000 
    armTimerRegAddr[ARM_TIMER_LOAD] = interval;
}

void writeARMtimerReg(word reg, word value) {
    armTimerRegAddr[reg] = value;
}

