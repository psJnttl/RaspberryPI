#ifndef ARM_TIMER_H
#define ARM_TIMER_H
#include "port.h"
#define ARM_TIMER_BASE (RPI_PERIPHERAL_BASE + 0xB000 + 0x400)
enum{
ARM_TIMER_LOAD,         // 0x00
ARM_TIMER_VALUE,        // 0x04
ARM_TIMER_CONTROL,      // 0x08
ARM_TIMER_IRQ_CLR,      // 0x0C
ARM_TIMER_RAW_IRQ,      // 0x10
ARM_TIMER_MASKED_IRQ,   // 0x14
ARM_TIMER_RELOAD,       // 0x18
ARM_TIMER_PREDIV,       // 0x1C
ARM_TIMER_FREE_RUNNING  // 0x20
}armTimerRegisters;

/* Timer control register  offset 0x08
   3         2         1        
  10987654321098765432109876543210     21:00 PINs 32...53
  xxxxxxxxGGGGGGGGxxxxxxFeDxCxBBaX
x - 24-31 : unused
G - 16-23 : Free running counter pre-scaler. Freq = sys_clk/(prescale+1)
x - 10-15 : unused
F - 09    : 0-free running counter disabled, 1-free running counter enabled
e - 08    : 0-timers keep running if ARM in debug halted mode, 1-timers halted if ARM halted
D - 07    : 0-timer disabled, 1-timer enabled
x - 06    : not used
C - 05    : 0-timer interrupt disabled, 1-timer interrupt enabled
x - 04    : not used
B - 03-02: Pre-scale bits:
           00: pre-scale is clock/1 (ie. no prescaling)
           01: pre-scale is clock/16
           10: pre-scale is clock/256
           11: pre-scale is clock/1
a - 01   : 0-16bit, 1-23bit counters
    00   : not supported
*/
#define ARM_TIMERS_FREE_RUN    0x200
#define ARM_TIMERS_RUNNING     0x100
#define ARM_TIMER_ENABLE       0x80
#define ARM_TIMER_INT_ENABLE   0x20
#define ARM_TIMER_PRESCALE_256 0x08
#define ARM_TIMER_PRESCALE_16  0x04
#define ARM_TIMER_23BIT        0x02

extern void enableARMtimer(void);
extern void setARMtimerInterval(word interval);
extern void writeARMtimerReg(word reg, word value);

#endif /* ARM_TIMER_H */
