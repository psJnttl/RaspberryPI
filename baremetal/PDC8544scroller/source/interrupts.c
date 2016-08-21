#include "port.h"
#include "interrupts.h"
#include "armTimer.h"
#include "gpio.h"
#include "sysTimer.h"
#include "uart.h"

/*  ARMv7 reference manual in section B1.8 Exception handling section B1.8.1 (Table B1-3)
Much clearer version is in older doc: ARMv5 referece manual section 2.6
offset   interrupt vector        PSR[4-0]  mode
-------+------------------------+--------+-------
0x00   | Reset                  | 0x13   | Supervisor
0x04   | Undefined Instruction  | 0x1B   | Undefined mode
0x08   | Hypervisor call        | 0x17   | Virtual memory and/memory protection
0x0C   | Prefetch Abort         | 0x17   | Virtual memory and/memory protection
0x14   | Hyp trap / Hyp entry   |
0x18   | IRQ interrupt          | 0x12   | Interrupt mode
0x1C   | FIQ interrupt          | 0x11   | Fast Interrupt mode
--------------------------------+------------
see also
https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html#ARM-Function-Attributes
https://geekwentfreak-raviteja.rhcloud.com/blog/2011/01/16/writing-interrupt-routines-using-gcc-for-arm/
http://doc.ironwoodlabs.com/arm-arm-none-eabi/html/getting-started/sec-cs3-interrupts.html
*/
volatile word *intRegAddr = (word *)INTERRUPT_BASE; // base pointer for registry access
// reset goes to 0x8000 / _start:

// Undefined instruction
void __attribute__ ((interrupt ("UNDEF"))) undef_handler () {
}

// Software interrupt
void __attribute__ ((interrupt ("SWI"))) swi_handler () {
}

// Instruction Prefetch Abort
void __attribute__ ((interrupt ("ABORT"))) prefetch_handler () {
}

// Data Access Memory Abort
void __attribute__ ((interrupt ("ABORT"))) dataAbort_handler () {
}
volatile char uartIrqChar=0;
volatile byte timeI = 0, uartI=0;
volatile word gpedsValue = 0;
// Interrupt
void __attribute__ ((interrupt ("IRQ"))) irq_handler () {
    word interruptID = intRegAddr[IRQ_BASIC], regVal=0; // 0x01 == ARM_TIMER_IRQ
    if(interruptID & ARM_TIMER_IRQ ) {
        writeARMtimerReg(ARM_TIMER_IRQ_CLR, 1);  // clear or we'll be back very quickly
        getTimeNow(1);
        timeI = 1;
    }
    interruptID = intRegAddr[IRQ_2]; // GPIO, UART etc.

    if(interruptID & (1 << (UART_INT - 32)) ) { // UART interrupt
        gpedsValue = readUARTregister(UART_MIS); // reading clears MIS

        if (gpedsValue & UART_TX_INTERRUPT) { // rx == 0x20
             // can RX and TX interrupt happen at the same time?
            uartI = 0x40; // signal kernel
            while((UARTtxIn != UARTtxOut) && (!(readUARTregister(UART_FR) & UART_TX_FIFO_FULL)) )  {
                writeUARTregister(UART_DR, UARTtxBuffer[UARTtxOut]); // UART tx buffer not empty and UART FIFO not full
                UARTtxOut = (UARTtxOut + 1) % UART_BUFFER_SIZE; // wrap around
            }
            writeUARTregister(UART_ICR, UART_TX_INTERRUPT); // ack irq 
        } /* UART TX INTERRUPT */        
        else if(gpedsValue & UART_RX_INTERRUPT) { // rx == 0x10
            uartI = 0x80; // signal kernel
            while( !(readUARTregister(UART_FR) & UART_RX_FIFO_EMPTY)) { // read until FIFO empty
                uartIrqChar = readUARTregister(UART_DR);
                if( ((UARTrxIn + 1) % UART_BUFFER_SIZE == UARTrxOut) ) { // oops, buffer full
                    break; // let's leave this party
                }      // if ring buffer full char will be discarded
                UARTrxBuffer[UARTrxIn] = uartIrqChar; 
                UARTrxIn = (UARTrxIn + 1) % UART_BUFFER_SIZE;
            }
            writeUARTregister(UART_ICR, UART_RX_INTERRUPT); // ack irq, also reading DR 
        } /* UART_RX_INTERRUPT */
    } /* UART_INT */
}

// Fast Interrupt
void __attribute__ ((interrupt ("FIQ"))) fiq_handler () {
}

void enableInterruptsByType(word type, word interruptMask) {
 /* IRQ_BASIC, IRQ_1, IRQ_2, FIQ_CNTL, IRQ_ENABLE_1, IRQ_ENABLE_2,
     IRQ_ENABLE_BASIC, IRQ_DISABLE_1, IRQ_DISABLE_2, IRQ_DISABLE_BASIC */
    intRegAddr[type] = interruptMask; // |= not used because disable done with other register
}

void disableInterruptsByType(word type, word interruptMask) {
    intRegAddr[type] = interruptMask;
}

word readInterruptStatus(word type) {
    return intRegAddr[type];
}

void enableInterrupts(byte intMask) { // r0= intMask
    asm("MRS  R1, CPSR");    // 0x80-IRQ, 0x40-FIQ
    asm("BIC  R1, R1, R0"); // 0-enabled, 1-disabled
    asm("MSR  CPSR_c, R1");
}

void disableInterrupts(byte intMask) { // r0 has intMask
    asm("MRS  R1, CPSR");
    asm("ORR  R1, R1, R0"); // 1-disabled
    asm("MSR  CPSR_c, R1");
}

