#include "port.h"
#include "gpio.h"
#include "uart.h"
#include "interrupts.h"

char UARTrxBuffer[UART_BUFFER_SIZE]; // internal ring
char UARTtxBuffer[UART_BUFFER_SIZE]; // buffers
volatile int UARTtxIn = 0, UARTtxOut = 0, UARTrxIn = 0, UARTrxOut = 0; // buffer pointers
volatile word *uartRegAddr = (word *)UART_BASE;  // base pointer for registry access

void init_UART(void) { // no args for now

    configureGPIOpin(14, GPIO_FS_ALTERNATE_0);  // TXD0
    configureGPIOpin(15, GPIO_FS_ALTERNATE_0);  // RXD0
// UART must be disabled before writing to LCRH  (& CR)
    uartRegAddr[UART_CR] = 0x00; // UART disable, CR step 1
    while(uartRegAddr[UART_FR] & 0x08) {}  // While UART busy, CR step 2
// configure TX and RX interrupts on
    uartRegAddr[UART_IMSC] = (UART_RX_INTERRUPT | UART_TX_INTERRUPT); // 0-disable interrupts

/* FIFOs disabled, lowest level is two bytes-> terminal'll be choppy
    uartRegAddr[UART_IFLS] = 0x00; // 0==1/8, 001001 1/4 RX & TX, ie. 4bytes; default: 492h  010010 RX 010== half, TX 010 == half*/

/* BAUDDIV = (FUARTCLK/(16 Baud rate))
FUARTCLK/Mhz  int  frac
    3          1    41   1.62760416667 > .62760416667 x 64 + 0.5 = 40.66
    4          2    11
  7.3728       4     0
 default      1     28h   these are the default values in registers, FUARTCLK == ~3MHz
RPi2 has 3MHz UART clock by default: https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=98904&hilit=UART
(Baud rate) x (BAUDDIV x 16) = FUARTCLK 
*/
    uartRegAddr[UART_IBRD] = 1;      // Integer baud rate divisor
    uartRegAddr[UART_FBRD] = 40;     // Fractional baud rate divisor
    uartRegAddr[UART_LCRH] = 0x60; // no parity, 8bit data, FIFOs disabled, 1stop bit, break
    uartRegAddr[UART_CR] = 0x301; //  CTS&RTS Off, UART enable, CR steps 4 & 5
}

word readUARTregister(word reg) {
    return uartRegAddr[reg];
}

void writeUARTregister(word reg, word value) {
    uartRegAddr[reg] = value;
}

byte readByteFromUART(void) {
    if( (uartRegAddr[UART_FR] & 0x10) == 0) { // RX FIFO has smthn
        return (uartRegAddr[UART_DR] & 0xFF); // omit error bits
    }
    return 0;
}

void writeByteToUART(byte c) {
    if( (uartRegAddr[UART_FR] & 0x20) ) { // if TX FIFO full
        ;                                 // char is lost
    }
    else uartRegAddr[UART_DR] = c;
}

/* Ring buffer for UART-----------------------------------------------------------*/
char readCharFromUARTrxBuffer(void) {
    char uartChar = 0;
    disableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) ); // AUX IRQ OFF
    if(UARTrxIn != UARTrxOut) { // buffer not empty
        uartChar = UARTrxBuffer[UARTrxOut];
        UARTrxOut = (UARTrxOut + 1) % UART_BUFFER_SIZE;
    }
    enableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) ); // AUX IRQ ON
    return uartChar;
}

void insertUARTtxBuffer(char *cptr) {
    int i = 0; 
    word IRQmask = readInterruptStatus(IRQ_ENABLE_2);
    disableInterruptsByType(IRQ_DISABLE_2, (1 << (UART_INT-32)) ); // UART IRQ OFF
    while(cptr[i] != '\0' )  { // until end of string 
        if( ((UARTtxIn+1) % UART_BUFFER_SIZE == UARTtxOut) ) { //if TXring buf is full, make room by enabling TX irq (RX also...)
            uartRegAddr[UART_IMSC] = ( UART_TX_INTERRUPT); // only TX interrupt enabled
            enableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) );
            while( ((UARTtxIn+1) % UART_BUFFER_SIZE == UARTtxOut) ) {} // wait till there's room
            disableInterruptsByType(IRQ_DISABLE_2, (1 << (UART_INT-32)) );
            uartRegAddr[UART_IMSC] = (UART_RX_INTERRUPT | UART_TX_INTERRUPT); // TX and RX both again
        }
        UARTtxBuffer[UARTtxIn] = cptr[i++];
        UARTtxIn = (UARTtxIn + 1) % UART_BUFFER_SIZE; // wrap around
    }
    if ( (!(uartRegAddr[UART_FR] & UART_TX_FIFO_FULL)) && (UARTtxIn != UARTtxOut)) { // TX has room, ringBuffer not empty
        uartRegAddr[UART_DR] = UARTtxBuffer[UARTtxOut]; // 1st UART write. Next takes place in IRQ handler
        UARTtxOut = (UARTtxOut + 1) % UART_BUFFER_SIZE; //
    }
    enableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) ); // UART IRQ ON
    enableInterruptsByType(IRQ_ENABLE_2, IRQmask); // put EVERYTHING back in place
}
/* /Ring buffer for UART-----------------------------------------------------------*/

void writeLongToUARTasDec(unsigned long c) {
    char result[11], rIdx=0, i=0, j=0;
    long div=100000000L, mod=1000000000L;
    result[rIdx++] = c / 1000000000L + 0x30;
    while(1) {
        result[rIdx++] = ((c%mod) /div) + 0x30;
        if(div == 1) break;
        mod /= 10; div /= 10;
    }
    result[10]='\0';
    for(i=0; i<11; i++) { // prefixing zeroes have to go
        if(result[i] != '0') break; // i marks the spot
    }
    for(j=0; i<10; i++,j++) {  // j ehto poistettu tarpeettomana
        result[j] = result[i];
    }
    if(i==10 && j==0) {  // it's zero, let's print it too.
        result[0] = 0x30;
        j=1;
    }
    result[j] = '\0';
    insertUARTtxBuffer(result);
}
