#ifndef SPI_H
#define SPI_H
#include "port.h"
#define SPI_BASE  (RPI_PERIPHERAL_BASE + 0x200000 + 0x4000)

enum {
AUX_SPI0_CS,   // 0x00
AUX_SPI0_FIFO, // 0x04
AUX_SPI0_CLK,  // 0x08
AUX_SPI0_DLEN, // 0x0C  // valid for DMA mode
AUX_SPI0_LTOH, // 0x10  // LoSSI output hold delay
AUX_SPI0_DC    // 0x14  // valid for DMA mode
} spiRegs;

extern void SPIinit(void);
 
#endif /* SPI_H */
