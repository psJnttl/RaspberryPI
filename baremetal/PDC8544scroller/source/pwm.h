#ifndef PWM_H
#define PWM_H
#include "port.h"

#define PWM_BASE (RPI_PERIPHERAL_BASE + 0x20C000)
#define PWM_CLOCK_BASE  (RPI_PERIPHERAL_BASE + 0x101000 + 0xA0)

enum {
PWM_CTL,    // 0x00 control
PWM_STA,    // 0x04 status
PWM_DMAC,   // 0x08 DMA configuration
PWM_reserved1,
PWM_RNG1,   // 0x10 CH1 range
PWM_DAT1,   // 0x14 CH1 data
PWM_FIF1,   // 0x18 FIFO input
PWM_reserved2,
PWM_RNG2,   // 0x20 CH2 range
PWM_DAT2,   // 0x24 CH2 data
} pwmRegs;

/*  9.6 Control and Status Registers  CTL Register
  10987654321098765432109876543210         10987654321098765432109876543210       ch1 in PWM mode, PWM algo used, normal polarity
  xxxxxxxxxxxxxxxxAbCdEfGhIjKlMnOp         00000000000000000000000000000001
x - 31:16 reserved                         xxxxxxxxxxxxxxxxAbCdEfGhIjKlMnOp
A - 15 MSEN2: Channel 2 M/S Enable, 0-PWM used, 1-M/S transmission used  (mark space)
b - 14 reserved
C - 13 USEF2: Ch 2 Use FIFO, 0-data register is transmitted, 1-FIFO is used for transmission
d - 12 POLA2: Ch 2 Polarity, 0-low==0;high==1, 1-low==1;high==0
E - 11 SBIT2: Ch 2 Silence Bit, defines the state of output when no transmission takes place
f - 10 RPTL2: Ch 2 Repeat Last Data, 0-transmission interrupts when FIFO empty,1-Last data in FIFO is transmitted until FIFO not empty
G -  9 MODE2: Ch 2 Mode: 0-PWM mode, 1-Serialiser mode
h -  8 PWEN2: Ch 2 Enable, 0-Ch disabled, 1-Ch enabled
I -  7 MSEN1: Channel 1 M/S Enable, 0-PWM used, 1-M/S transmission used  (mark space)
j -  6 CLRF1: Clear FIFO, 0-no effect, 1-Clears FIFO   a single shot operation
K -  5 USEF1: Ch 1 Use FIFO, 0-data register is transmitted, 1-FIFO is used for transmission
l -  4 POLA1: Ch 1 Polarity, 0-low==0;high==1, 1-low==1;high==0
M -  3 SBIT1: Ch 1 Silence Bit, defines the state of output when no transmission takes place
n -  2 RPTL1: Ch 1 Repeat Last Data, 0-transmission interrupts when FIFO empty,1-Last data in FIFO is transmitted until FIFO not empty
O -  1 MODE1: Ch 1 Mode: 0-PWM mode, 1-Serialiser mode
p -  0 PWEN1: Ch 1 Enable, 0-Ch disabled, 1-Ch enabled   */

/*  9.6 Control and Status Registers  RNG1  Range Register  and RNG2 for ch2
  10987654321098765432109876543210         10987654321098765432109876543210
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA         00000000000000000000000000000000
A - 31:00 PWM_RNGi Channel i Range         AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
In PWM mode evenly distributed pulses are sent within a period of length defined by this register.
Serialised data is transmitted within the same period. If value < 32 only the 1st PWM_RNGi bits are 
sent resulting in a truncation. If value > 32 excess zero bits are padded at the end of data.
Default value for register is 32.   */

/*  9.6 Control and Status Registers  DAT1  Range Register  and DAT2 for ch2
  10987654321098765432109876543210         10987654321098765432109876543210
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA         00000000000000000000000000000000
A - 31:00 PWM_DATi Channel i Data          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Stores 32 bit data to be sent by PWM Controller when USEFi is 0, ie. FIFO not used.
In PWM mode data is sent by pulse width modulation: the value of this register defines the number of 
pulses which is sent within the period defined by PWM_RNG1. In serialiser mode data stored in this 
register is serialised and transmitted.   */

enum {             // 0xA0 from clock base
PWMCLK_CNTL,       // 0x00
PWMCLK_DIV         // 0x04
} pwmClockRegs; 

#define PWMCLK_PASSWD       0x5A000000
#define PWMCLK_MASH_3       0x600
#define PWMCLK_MASH_2       0x400
#define PWMCLK_MASH_1       0x200
#define PWMCLK_FLIP         0x100
#define PWMCLK_BUSY         0x80
#define PWMCLK_KILL         0x20
#define PWMCLK_ENABLE       0x10
#define PWMCLK_SCR_HDMI     7
#define PWMCLK_SCR_PLLC200  6
#define PWMCLK_SCR_PLLD400  5
#define PWMCLK_SCR_PLLA650  4
#define PWMCLK_SCR_TEST1    3
#define PWMCLK_SCR_TEST0    2
#define PWMCLK_SCR_19M2     1
#define PWMCLK_SCR_GND      0
/*  [PWMCLK_CNTL] PWM clock control register
the same function as General Purpose clocks defined in BCM2835 Peripherals 
section 6.3 General Purpose GPIO Clocks  Clock Manager
   3         2         1                    3         2         1
  10987654321098765432109876543210         10987654321098765432109876543210
  AAAAAAAAxxxxxxxxxxxxxBBcDxEfGGGG         XXXXXXXXXXXXXXXXXXXXXXXX1X1XXXXX
A - 31:24 PASSWD: Clock manager password 0x5a (0x5A000000)
x - 23:11 Unused
B - 10:9  MASH: Mash ctrl, 0-int division,1-1stage MASH,2-2stage MASH, 3-3stage MASH
          Do not change this while Busy == 1
c - 8 FLIP: Invert clock generator output
D - 7 BUSY: Indicates if clock generator is running
x - 6 Unused
E - 5 KILL: Kill the clock generator, 0-no action, 1- stop and reset clock generator
f - 4 ENAB: Enable the clock generator, start or stop (wait for busy)
G - 3:0 SRC: Clock source
        0 - GND                          5 - PLLC per   400MHz
        1 - oscillator 19.2MHz,apparentl 6 - PLLD per   200MHz
        2 - testdebug0                   7 - HDMI auxiliary
        3 - testdebug1                   8-15 - GND
        4 - PLLA per   650MHz          wait for BUSY

[PWMCLK_DIV] Clock manager divisors
   3         2         1                    3         2         1
  10987654321098765432109876543210         10987654321098765432109876543210
  AAAAAAAAbbbbbbbbbbbbCCCCCCCCCCCC         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
A - 31:24 PASSWD: Clock manager password 0x5a
b - 23:12 DIVI: integer part of divisor, min. determined by MASH
C - 11:0  DIVF: Fractional part of divisor
*/

extern void initPWM(void);
extern void toggleLCDbacklight(void);
extern void printLCDbrightnessMenuToUART(void);
#endif /* PWM_H */
