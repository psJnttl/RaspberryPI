#ifndef UART_H
#define UART_H
#include "port.h"
#define UART_BUFFER_SIZE 100
#define UART_BASE (RPI_PERIPHERAL_BASE + 0x201000)

enum {
UART_DR,        // 0x00 Data register
UART_RSRECR,    // 0x04 Receive status / error clear
UART_FR=0x06,   // 0x18 Flag register
UART_ILPR=0x08, // 0x20 Not in use (IrDA low power)
UART_IBRD,      // 0x24 Integer baud rate divisor
UART_FBRD,      // 0x28 Fractional baud rate divisor
UART_LCRH,      // 0x2c Line control register
UART_CR,        // 0x30 Control register
UART_IFLS,      // 0x34 Interrupt FIFO level select
UART_IMSC,      // 0x38 Interrupt mask set clear
UART_RIS,       // 0x3c Raw interrupt status 
UART_MIS,       // 0x40 Masked interrupt status
UART_ICR,       // 0x44 Interrupt clear
UART_DMACR,     // 0x48 DMA control
UART_ITCR=0x20, // 0x80 Test control
UART_ITIP,      // 0x84 Integration test input
UART_ITOP,      // 0x88 Integration test output
UART_TDR        // 0x8c Test data register
} UARTregisters;

/*   [UART_FR]  Flag Register
   3         2         1        
  10987654321098765432109876543210
  00000000000000000000000aBcDeFgHi
31-9  Reserved
a - 8   RI   unsupported (Ring Indicator)
B - 7   TXFE Transmit FIFO empty
c - 6   RXFF Receive FIFO full
D - 5   TXFF Transmit FIFO full
e - 4   RXFE Receive FIFO empty
F - 3   BUSY UART busy. 1-UART transmitting data
g - 2   DCD  unsupported
H - 1   DSR  unsupported
i - 0   CTS  Clear To Send.  nUARTCTS complement */
#define UART_TX_FIFO_FULL  0x20
#define UART_RX_FIFO_EMPTY 0x10

/*[UART_LCRH]  Line Control Register   BCM2835 ARM peripherals  p184-185
   3         2         1        
  10987654321098765432109876543210
  000000000000000000000000aBBcDeFg
                          01110001  0x71 current configuration
31-8  reserved
a - 7   SPS  Parity select: 0-disabled, 1-check EPS: 0-trans&check as 1, EPS:1-trans&check as 0
B - 6-5 WLEN Word length: 11-8bits, 10-7bits, 01-6bits, 00-5bits
c - 4   FEN  Enable FIFOs:  0-disable (1byte holding reg), 1-enable 
D - 3   STP2 Two stop bits select: 1-2 stop bits sent, recv doesn't check 2 stop bits
e - 2   EPS  Even parity select:  0-odd, 1-even parity  note PEN
F - 1   PEN  Parity enable: 0-disable, 1-enabled
g - 0   BRK  Send break: 1- low continually sent on the TXD after transmission


[UART_CR]  Control Register
   3         2         1        
  10987654321098765432109876543210
  0000000000000000aBcDeFgHi0000JkL
                  0000001100000001 0x301 current configuration
31-16  reserved
a - 15   CTSEN  CTS HW flow control: 1-enabled, 0-disabled. Data only sent if nUARTCTS low
B - 14   RTSEN  RTS HW flow control: 1-enabled, 0-disabled. Data only requested if room in FIFO
c - 13   OUT2   unsupported (uartout2 complement, DTE: Ring Indicator)
D - 12   OUT1   unsupported (uartout1 complement, DTE: Data Carrier Detect)
e - 11   RTS    Request To Send, complement of UART request to send nUARTRTS
F - 10   DTR    unsupported (Data Transmit Ready)
g -  9   RXE    Receive Enable: 1-UART RX enabled, 0-UART RX disabled
H -  8   TXE    Transmit Enable: 1-UART TX enabled, 0-UART TX disabled
i -  7   LBE    Loopback Enable: 1-TXD fed into RXD, 0-disabled ie. normal operation
   6-3   reserved
J -  2   SIRLP  unsupported (SIR low power IrDA)
k -  1   SIREN  unsupported (SIR enable)
L -  0   UARTEN UART Enable:  0-disabled, 1-enabled


[UART_IMSC]  Interrupt Mask Set Clear register
   3         2         1        
  10987654321098765432109876543210
  000000000000000000000aBcDeFgHiJk
31-11  Reserved
a - 10  OEIM   Overrun Error
B -  9  BEIM   Break Error
c -  8  PEIM   Parity Error
D -  7  FEIM   Framing Error
e -  6  RTIM   Receive timeout
F -  5  TXIM   Transmit
g -  4  RXIM   Receive 
H -  3  DSRMIM Unsupported
i -  2  DCDMIM Unsupported
J -  1  CTSMIM nUARTCTS modem interrupt
k -  0  RIMIM  Unsupported
*/
#define UART_TX_INTERRUPT  0x20
#define UART_RX_INTERRUPT  0x10

extern void init_UART(void);
extern void writeByteToUART(byte c);
extern byte readByteFromUART(void);
extern word readUARTregister(word reg);
extern void writeUARTregister(word reg, word value);
// for interrupts:
extern char UARTrxBuffer[UART_BUFFER_SIZE];
extern char UARTtxBuffer[UART_BUFFER_SIZE];
extern volatile int UARTtxIn, UARTtxOut, UARTrxIn, UARTrxOut;
extern void insertUARTtxBuffer(char *cptr);
extern char readCharFromUARTrxBuffer(void);

extern void writeLongToUARTasDec(unsigned long c);
#endif /* UART_H */
