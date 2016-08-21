#include "port.h"
#include "gpio.h"
#include "sysTimer.h"
#include "uart.h"
#include "interrupts.h"
#include "armTimer.h"
#include "spi.h"
#include "pdc8544.h"
#include "pwm.h"


static int LEDstate = 1;

void printMenuToUART(void) {
    insertUARTtxBuffer("------------------------------\r\n\0");
    insertUARTtxBuffer(" 1 - Set time\r\n\0");
    insertUARTtxBuffer(" 2 - Get time now\r\n\0");
    insertUARTtxBuffer(" 3 - Set scroll message\r\n\0");
    insertUARTtxBuffer(" 4 - LCD backlight brightness\r\n> \0");
}

void kernel_main(void) {
    unsigned long loop=0;
    unsigned char testString[9] = {0,0,0,0,0,0,0,0,0}, testChar=0, dataToSend=0;
    configureGPIOpin(OK_LED, GPIO_FS_OUTPUT);  //  LED is connected to PIN47 on RPi2
    for(loop=0; loop < 500; loop++);
// UART
    init_UART();
    enableInterruptsByType(IRQ_ENABLE_2, (1 << (UART_INT-32)) ); // UART interrupts

// LCD
    SPIinit();
    pdc8544_init();
// PWM  acts as LCDs backlight control, GPIO18
    initPWM();
// scroller
    initScroller("Just testing...          \0", 0);


// ARM timer
    setARMtimerInterval(50000); // 1000000== 1/s
    enableARMtimer(); // pre-scaler always 256
    enableInterruptsByType(IRQ_ENABLE_BASIC, ARM_TIMER_IRQ); // specific for ARM timer
// Interrupts master: FIQ, IRQ
    enableInterrupts(IRQ_MASK); // IRQs in general

    writeToGPIOpin(OK_LED, LEDstate);
    printMenuToUART();
    while(1) {
        if(timeI == 1) {
            setTimeNow2baseTime();
            timeNowToPDC8544(5); // line 5
            timeI = 0;
            operateScroll();
            updateScrollBuffer();
        }

        if(uartI == 0x80) {  // UART RX
            testChar = readCharFromUARTrxBuffer();
            if(testChar != 0)  {
                testString[0] = testChar;
                insertUARTtxBuffer(testString);     // echo it right back
                if(testChar == '1') {
                    getTimeLoop();
                }
                else if (testChar == '2') {
                    insertUARTtxBuffer("\r\nTime now: \0");
                    getTimeNow(1);
                    setTimeNow2baseTime();
                    timeNowToUART(1);
                }
                else if (testChar == '3') {
                    getScrollMsg();
                }
                else if (testChar == '4') {
                    printLCDbrightnessMenuToUART();
                }
                printMenuToUART();
            }
            else {
                writeByteToUART(127);  // backspace
            }
            clearUARTrxBuffer();
            uartI = 0;
        } /* UART RX */
    } /* while */
} // kernel_main


