#include "port.h"
#include "gpio.h"
#include "spi.h"
#include "pdc8544.h"

volatile word *spiRegAddr = (word *)SPI_BASE;  // base pointer for registry access

void SPIinit(void) {
unsigned int regVal = 0;

//alt0  -> GPIO09 == SPI0 MISO   , MISO not connected to PDC8544
//alt0  -> GPIO08 == SPI0_CE0  Chip Enable, normally Slave 1 /CE
    configureGPIOpin(8, GPIO_FS_ALTERNATE_0);
    configureGPIOpin(7, GPIO_FS_OUTPUT);
// GPIO10   alt0 -> GPIO10 == SPI0 MOSI
    configureGPIOpin(10, GPIO_FS_ALTERNATE_0);
// GPIO11  alt0 -> GPIO11 == SPI0 SCLK
    configureGPIOpin(11, GPIO_FS_ALTERNATE_0);
// -> GPIO25 == output,   Data/Command select
    configureGPIOpin(25, GPIO_FS_OUTPUT);

    spiRegAddr[AUX_SPI0_CS] =  0x0000030;    // clear FIFOs
/*  10.5 SPI Register Map
XX204000  SPI Master Control and Status
                                                                   10110000
  10987654321098765432109876543210         10987654321098765432109876543210
  xxxxxxAbCdEfGhIjKlMnOpQrStUUvWxx         00000000000000000000000000110000
x - 31:26 reserved                         xxxxxxAbCdEfGhIjKlMnOpQrStUUvWxx
A - 25 LEN_LONG: Lossi mode, 0-writing single byte to FIFO, 1-writing 32bits to FIFO
b - 24 DMA_LEN: Enable DMA mode in Lossi mode
C - 23 CSPOL2: Chip Select 2 Polarity, 0-active low, 1-active high
d - 22 CSPOL1: Chip Select 1 Polarity, 0-active low, 1-active high
E - 21 CSPOL0: Chip Select 0 Polarity, 0-active low, 1-active high
f - 20 RXF: RX FIFO Full: 0-not, 1-full
G - 19 RXR: RX FIFO needs reading, 0-RX FIFO less than full, 1-RX FIFO full
h - 18 TXD: TX FIFO can accept data, 0-full, can't accept, 1-FIFO has space for at least 1 byte
I - 17 RXD: RX FIFO contains data, 0-FIFO empty, 1-FIFO contains at least 1 byte
j - 16 DONE: transfer done, 0-transfer in progress, 1-transfer complete
K - 15 TE_EN: unused
l - 14 LMONO: unused
M - 13 LEN: LoSSI enable, 0-serial inteface will behave as SPI master, 1-LoSSI master
n - 12 REN: read enable, 0-intend to write to SPI peripheral, 1-intend to read
O - 11 ADCS: Automatically Deassert Chip Select, 0-don't, 1-auto deassert CS at the end of DMA transfer (as determined by SPIDLEN)
P - 10 INTR: Interrupt on RXR, 0-don't gen int on RX FIFO condition, 1-generage interrupt while RXR=1
Q -  9 INTD: Interrupt on Done, 0-dont gen int on transfer complete, 1-generage interrupt when DONE=1
r -  8 DMAEN: DMA Enable, 0-no DMA requests will be issued, 1-enable DMA operation
S -  7 TA: transfer active, 0-not active, 1-transfer active
t -  6 CSPOL: Chip Select polarity, 0-CS lines active low, 1-CS lines active high
U -  5:4 CLEAR, 00-no action, x1-clear TX FIFO one shot operation, 1x-clear RX FIFO one shot operation
v -  3 CPOL: Clock polarity, 0-rest state of clock low, 1-rest state of clock high
W -  2 CPHA: Clock phase, 0-1st SCLK transtion in the middle of data bit, 1-1st SCLK transition at the beginning of data bit
x -  1:0 CS: Chip Select
     00 chip select 0     10 chip select 2
     01 chip select 1     11 chip select 3  
     
CPOL 0 ja CPHA 0 is ideal for PDC8544
*/
     
    spiRegAddr[AUX_SPI0_CLK] = 64;    // clock divider
/* SPI0 , chapter 10.5
CLK Register:  15:0 CDIV,  Clock divider, SCLK = Core clock / CDIV
150/38 = 3.95MHz  PDC8544 can do 4Mbit/s
nearest power of two that doesn't exceed 4MHz -> 2^6 = 64  -> 2.34MHz

10.5 SPI Register Map
XX204008  SPI Master Clock Divider

  10987654321098765432109876543210         10987654321098765432109876543210
  xxxxxxxxxxxxxxxxAAAAAAAAAAAAAAAA         00000000000000000000000000100110
x - 31:16 reserved                         xxxxxxxxxxxxxxxxAAAAAAAAAAAAAAAA
A - CDIV: Clock divider   SCLK = core clock / CDIV
    If CDIV is set to 0, the divisor is 65536. Divisor must be a power of 2, odd numbers rounded down
    SPI clock rate if of the APB clock.
*/
}

void SPIsendByte (unsigned int b) {
unsigned int regVal = 0;
// TransferActive=1, (CS lines active low-CSPOL=0), clear RX & TX FIFOs, chip select 0
    spiRegAddr[AUX_SPI0_CS] = 0x000000B0;
    while(1) { 
        regVal = spiRegAddr[AUX_SPI0_CS];
        if(regVal & (1 << 18)) { // wait until TX FIFO has room
            break; 
        }
    }
    spiRegAddr[AUX_SPI0_FIFO] = (b & 0xFF); // AND 0xFF: one byte to FIFO at 0x3F204004
    while(1) {
        regVal = spiRegAddr[AUX_SPI0_CS];
        if(regVal & (1 << 16)) {   // wait until transfer complete  bit16 == DONE
            break;
        }
    }
    spiRegAddr[AUX_SPI0_CS] =  0x00000000; //cs0 comes back up
}

/*--------------------------------------------------------------------------------*/
void SPIcommand ( unsigned int cmd ) {
    writeToGPIOpin(25, LCD_COMMAND);
    SPIsendByte(cmd);
}

void SPIdata ( unsigned int data ) {
    writeToGPIOpin(25, LCD_DATA);
    SPIsendByte(data);
}

