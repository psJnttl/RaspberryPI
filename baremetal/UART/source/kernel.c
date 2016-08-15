#include "port.h"
#include "gpio.h"
#include "sysTimer.h"
#include "uart.h"
#include "interrupts.h"

static int LEDstate = 1;

void printMenuToUART(void) {
    insertUARTtxBuffer("------------------------------\r\n\0");
    insertUARTtxBuffer(" 1 - Set time\r\n\0");
    insertUARTtxBuffer(" 2 - Get time now\r\n\0");
    insertUARTtxBuffer(" 3 - Toggle OK LED\r\n\0");
}

void kernel_main(void) {
    unsigned long loop=0;
    unsigned char testString[9] = {0,0,0,0,0,0,0,0,0}, testChar=0, dataToSend=0;
    configureGPIOpin(OK_LED, GPIO_FS_OUTPUT);  //  LED is connected to PIN47 on RPi2
    for(loop=0; loop < 500; loop++);
// UART
    init_UART();
    enableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) ); // UART interrupts

// Interrupts master: FIQ, IRQ
    enableInterrupts(IRQ_MASK); // IRQs in general

    writeToGPIOpin(OK_LED, LEDstate);
    printMenuToUART();
    loop=0;
    while(1) { // clock update, display LCD, menu for user
        if(uartI == 0x80) {  // UART RX
            testChar = readCharFromUARTrxBuffer();
            if(testChar != 0)  {
                testString[0] = testChar;
                insertUARTtxBuffer(testString);     // echo it right back
                if(testString[0] == '*') printMenuToUART();
                else if(testChar == '1') {
                    getTimeLoop();
                    printMenuToUART();
                }
                else if (testChar == '2')  {
                    insertUARTtxBuffer("\r\n\0");
                    getTimeNow(1);
                    setTimeNow2baseTime();
                    timeNowToUART(1);
                    printMenuToUART();
                }
                else if (testChar == '3')  {
                    LEDstate ^= 1;
                    writeToGPIOpin(OK_LED, LEDstate);
                }
            }
            else { 
                insertUARTtxBuffer("\x7f"); // backspace
            }
            uartI = 0; // let's trust buffer is empty
        } /* UART RX */
    } /* while */
} // kernel_main


