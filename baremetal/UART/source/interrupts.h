#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include "port.h"
/* BCM2835-ARM_Peripherals.pdf  7.5 Registers */
#define INTERRUPT_BASE (RPI_PERIPHERAL_BASE + 0xB000 + 0x200)

#define IRQ_MASK 0x80  // to be used when
#define FIQ_MASK 0x40  // accessing CPSR

enum {  // There's 10 interrupt registers
IRQ_BASIC,        // 0x00
IRQ_1,            // 0x04  IRQ pending 1
IRQ_2,            // 0x08  IRQ pending 2
FIQ_CNTL,         // 0x0C
IRQ_ENABLE_1,      // 0x10
IRQ_ENABLE_2,      // 0x14
IRQ_ENABLE_BASIC, // 0x18
IRQ_DISABLE_1,     // 0x1C
IRQ_DISABLE_2,     // 0x20
IRQ_DISABLE_BASIC // 0x24
} IntRegisters;

/* [IRQ_BASIC] Basic pending register 
bits 31-21 not used, bits 20-10 are GPU interrupts and 
9-8 are "There are some interrupts pending which you don't know about. 
They are in pending register 1 /2."
7-0 bits are 'normal' interrupts  */

//BasicPendingIRQ
#define ARM_TIMER_IRQ         0x01
#define ARM_MAILBOX_IRQ       0x02
#define ARM_DOORBELL_0_IRQ    0x04
#define ARM_DOORBELL_1_IRQ    0x08
#define GPU0_HALTED_IRQ       0x10
#define GPU1_HALTED_IRQ       0x20
#define ILLEGAL_ACCESS_1_IRQ  0x40
#define ILLEGAL_ACCESS_0_IRQ  0x80

//  [IRQ_1] [IRQ_2] GPU register bits not defined in this header

// [FIQ_CNTL] FIQ register,  bits 31-8 not used, bits 6-0 are FIQ source
#define FIQ_ENABLE 7
// 0-63 are GPU interrupts
enum {
ARM_TIMER_FIQ=64,  // ARM timer interrupt
ARM_MAILBOX_FIQ,      // 65
ARM_DOORBELL_0_FIQ,   // 66
ARM_DOORBELL_1_FIQ,   // 67
GPU0_HALTED_FIQ,      // 68
GPU1_HALTED_FIQ,      // 69
ILLEGAL_ACCESS_1_FIQ, // 70
ILLEGAL_ACCESS_0_FIQ  // 71
} FIQsource;

// [IRQ_ENABLE_1] Interrupt enable register 1:  IRQ source, bits 31-0 ARM peripherals interrupts table
// [IRQ_ENABLE_2] Interrupt enable register 2:  IRQ source, bits 63-32
enum { // most of the bits are undefined
AUX_INTERRUPT = 29,  // IRQ_ENABLE_1, the only source 
I2C_SPI_SLV_INT=43,  // IRQ_ENABLE_2 has the rest
PWA_0_INT= 45,
PWA_1_INT,
SMI_INT = 48,
GPIO_INT_0,
GPIO_INT_1,
GPIO_INT_2,
GPIO_INT_3,
I2C_INT,
SPI_INT,
PCM_INT,
UART_INT = 57,       // IRQ_ENABLE_2  last source
} ARMperipheralInterrupts;

/* [IRQ_ENABLE_BASIC] Base Interrupt enable register
the BasicPendingIRQ values can be used. Bits 31-8 are unused. */

/* [IRQ_DISABLE_1] Interrupt disable register 1
IRQ sources 31-0:  use  ARMperipheralInterrupts enum  */

/* [IRQ_DISABLE_2] Interrupt disable register 2
IRQ sources 63-32:  use  ARMperipheralInterrupts enum  */

/* [IRQ_DISABLE_BASIC] Base disable register
the BasicPendingIRQ values can be used. Bits 31-8 are unused. */

extern void enableInterruptsByType(word type, word interruptMask);
extern void disableInterruptsByType(word type, word interruptMask);
extern void enableInterrupts(byte intMask);
extern void disableInterrupts(byte intMask);
extern volatile byte uartI;
//extern volatile char uartIrqChar;
extern word readInterruptStatus(word type); // 
#endif /* INTERRUPTS_H */
