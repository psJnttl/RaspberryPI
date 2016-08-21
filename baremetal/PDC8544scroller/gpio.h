#ifndef GPIO_H
#define GPIO_H
#include "port.h"
#define GPIO_BASE (RPI_PERIPHERAL_BASE + 0x200000)

/* BCM2835-ARM-Peripherals.pdf Table 6-2 GPIO Alternate function select register 0
     GPIO Pin function select        GPIO Pin function select
000  input                      100  alternate function 0
001  output                     101  alternate function 1
010  alternate function 5       110  alternate function 2
011  alternate function 4       111  alternate function 3 */
enum {
GPIO_FS_INPUT,
GPIO_FS_OUTPUT,
GPIO_FS_ALTERNATE_5,
GPIO_FS_ALTERNATE_4,
GPIO_FS_ALTERNATE_0,
GPIO_FS_ALTERNATE_1,
GPIO_FS_ALTERNATE_2,
GPIO_FS_ALTERNATE_3,
GPIO_FS_MAX
}gpio_function_select; 

/* 
GPFSEL0      Table 6-2 GPIO Alternate function select register 0  offset 0
   3         2         1        
  10987654321098765432109876543210    31:30 reserved
  00aaaBBBcccDDDeeeFFFgggHHHiiiJJJ
     |  |  |  |  |  |  |  |  |  |
GPIO09 08 07 06 05 04 03 02 01 00

GPFSEL1      Table 6-3 GPIO Alternate function select register 1  offset 4
   3         2         1        
  10987654321098765432109876543210    31:30 reserved
  00aaaBBBcccDDDeeeFFFgggHHHiiiJJJ
     |  |  |  |  |  |  |  |  |  | 
GPIO19 18 17 16 15 14 13 12 11 10 

GPFSEL2      Table 6-4 GPIO Alternate function select register 2  offset 8
   3         2         1        
  10987654321098765432109876543210     31:30 reserved
  00aaaBBBcccDDDeeeFFFgggHHHiiiJJJ
     |  |  |  |  |  |  |  |  |  | 
GPIO29 28 27 26 25 24 23 22 21 20 

GPFSEL3      Table 6-5 GPIO Alternate function select register 3  offset C
   3         2         1        
  10987654321098765432109876543210     31:30 reserved
  00aaaBBBcccDDDeeeFFFgggHHHiiiJJJ
     |  |  |  |  |  |  |  |  |  | 
GPIO39 38 37 36 35 34 33 32 31 30 

GPFSEL4      Table 6-6 GPIO Alternate function select register 4  offset 10
   3         2         1        
  10987654321098765432109876543210     31:30 reserved
  00aaaBBBcccDDDeeeFFFgggHHHiiiJJJ
     |  |  |  |  |  |  |  |  |  | 
GPIO49 48 47 46 45 44 43 42 41 40 

GPFSEL5      Table 6-7 GPIO Alternate function select register 4  offset 14
   3         2         1        
  10987654321098765432109876543210     31:12 reserved
  00000000000000000000AAAbbbCCCddd
                       |  |  |  |
                  GPIO53 52 51 50      */
/*
GPSET0      Table 6-8 GPIO PIN output set register 0              offset 1C
   3         2         1        
  10987654321098765432109876543210     31:00 PINs 0...31
  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 

GPSET1      Table 6-8 GPIO PIN output set register 1              offset 20
   3         2         1        
  10987654321098765432109876543210     21:00 PINs 32...53
  0000000000aaaaaaaaaaaaaaaaaaaaaa 
            ^  ^  ^      ^
           53 50 47     40
GPCLR0 & GPCLR1  same setup, offsets 28 & 2C
                                       */
/*#define GPSET0 7
#define GPSET1 8

#define GPCLR0 10
#define GPCLR1 11*/
enum {  // there's 41 in total   BCM2835-ARM-Peripherals.pdf   6.1 Register View
GPFSEL0,     // 0x00 GPIO Function Select 0
GPFSEL1,     // 0x04 GPIO Function Select 1
GPFSEL2,     // 0x08 GPIO Function Select 2
GPFSEL3,     // 0x0c GPIO Function Select 3
GPFSEL4,     // 0x10 GPIO Function Select 4
GPFSEL5,     // 0x14 GPIO Function Select 5
// 0x18<->24<-> 6th is reserved 
GPSET0 = 7,  // 0x1C GPIO Pin Output Set 0 
GPSET1,      // 0x20 GPIO Pin Output Set 1
// 0x24<->36-> 9th is reserved
GPCLR0=10,   // 0x2C GPIO Pin Output Clear 0
GPCLR1,      // 0x2C GPIO Pin Output Clear 1
//0x30 <-> 48 <-> 12: reserved
GPLEV0=13,   // 0x34 GPIO Pin Level 0
GPLEV1,      // 0x38 GPIO Pin Level 1
// 0x3C <-> 60 <-> 15: reserved
GPEDS0=16,   // 0x40 GPIO Pin Event Detect Status 0
GPEDS1,      // 0x44 GPIO Pin Event Detect Status 1
// 0x48 <-> 72 <-> 18: reserved
GPREN0=19,   // 0x4C GPIO Pin Rising Edge Detect Enable 0
GPREN1,      // 0x50 GPIO Pin Rising Edge Detect Enable 1
// 0x54 <-> 84 <-> 21: reserved
GPFEN0=22,   // 0x58 GPIO Pin Falling Edge Detect Enable 0
GPFEN1,      // 0x5C GPIO Pin Falling Edge Detect Enable 1
// 0x60 <->96 <-> 24: reserved
GPHEN0=25,   // 0x64 GPIO Pin High Detect Enable 0
GPHEN1,      // 0x68 GPIO Pin High Detect Enable 1
// 0x6C <-> 108 <-> 27: reserved
GPLEN0=28,   // 0x70 GPIO Pin Low Detect Enable 0
GPLEN1,      // 0x74 GPIO Pin Low Detect Enable 1
// 0x78 <-> 120 <-> 30: reserved
GPAREN0=31,  // 0x7C GPIO Pin Async. Rising Edge Detect 0 
GPAREN1,     // 0x80 GPIO Pin Async. Rising Edge Detect 1
// 0x84 <-> 132 <-> 33: reserved
GPAFEN0=34, // 0x88 GPIO Pin Async. Falling Edge Detect 0
GPAFEN1,    // 0x8C GPIO Pin Async. Falling Edge Detect 1
// 0x90<-> 144 <-> 36: reserved
GPPUD=37,   // 0x94 GPIO Pin Pull-up/down Enable
GPPUDCLK0,  // 0x98 GPIO Pin Pull-up/down Enable Clock 0
GPPUDCLK1   // 0x9C GPIO Pin Pull-up/down Enable Clock 1
} GPIOregisters;
enum {
GPIO_PIN_LOW_DETECT=0,
GPIO_PIN_HIGH_DETECT,
GPIO_PIN_EDGE_FALLING,
GPIO_PIN_EDGE_RISING,
GPIO_PIN_EDGE_ASYNC_FALLING,
GPIO_PIN_EDGE_ASYNC_RISING
} GPIOpinDetectEnum;
#define PWR_LED 35
#define OK_LED 47
extern void writeGPIOreg(byte GPIOreg, word data);
extern void configureGPIOpin(byte GPIOpin, byte mode);
extern void writeToGPIOpin(byte GPIOpin, byte data);
#endif
