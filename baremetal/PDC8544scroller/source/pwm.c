#include "port.h"
#include "pwm.h"
#include "gpio.h"
#include "uart.h"
#include "interrupts.h"

volatile word *pwmRegAddr = (word *)PWM_BASE;  // base pointer for registry access
volatile word *pwmClockRegAddr = (word *)PWM_CLOCK_BASE;

static pwmDATbackup = 0;

void initPWM(void) {   
    word i;
    configureGPIOpin(18, GPIO_FS_ALTERNATE_5); //GPIO18 alt5 = PWM0
    pwmClockRegAddr[PWMCLK_CNTL] = (PWMCLK_PASSWD | PWMCLK_KILL);
    while (pwmClockRegAddr[PWMCLK_CNTL] & PWMCLK_BUSY) {}
    pwmRegAddr[PWM_RNG1] = 100;                // 32 is the default. 100 for percentage accuracy
    pwmRegAddr[PWM_DAT1] = 10;                 // 10% dutycycle
    pwmClockRegAddr[PWMCLK_DIV] = (PWMCLK_PASSWD | ( 1920 << 12)); // set divisor for 1kHz on 19.2MHz oscillator
    pwmClockRegAddr[PWMCLK_CNTL] = (PWMCLK_PASSWD | (PWMCLK_ENABLE | PWMCLK_SCR_19M2));
    pwmRegAddr[PWM_CTL] = 0x01; // enable PWM0, PWM mode (bit0x80=0), Mark/Space bit0x80=1
}  

void toggleLCDbacklight(void) {
    if(pwmRegAddr[PWM_DAT1] > 0) { // On -> Off
        pwmDATbackup = pwmRegAddr[PWM_DAT1];
        pwmRegAddr[PWM_DAT1] = 0;
    }
    else { // Off -> On
        pwmRegAddr[PWM_DAT1] = pwmDATbackup;
        pwmDATbackup = 0;
    }
}

void printLCDbrightnessMenuToUART(void) { // assumes RNG == 100
byte LCDb[4]={'0',0,0,0}, uartChar=0,numChars=0, testString[9] = {0,0,0,0,0,0,0,0,0};
hword LCDbrighness;
    insertUARTtxBuffer("----------------------------------------\r\n\0");
    insertUARTtxBuffer("q - cancel\r\n>\0");
    insertUARTtxBuffer("Set LCD backlight brightness, range 0-100\r\n>\0");
    while(1) {
        if(uartI == 0x80) {
            uartChar = readCharFromUARTrxBuffer();

            if(uartChar >= '0' && uartChar <= '9' && numChars < 3) {
                testString[0] = uartChar;
                insertUARTtxBuffer(testString);
                LCDb[numChars++] = uartChar;
            }
            else if(uartChar == '\r') {
                LCDbrighness = atoi(LCDb);
                if(LCDbrighness>= 0 && LCDbrighness <= 100  && numChars != 0) {
                    pwmRegAddr[PWM_DAT1] = LCDbrighness;
                    insertUARTtxBuffer("\r\n\0");
                    if(pwmDATbackup > 0 && LCDbrighness == 0) pwmDATbackup = LCDbrighness; 
                    break;  // this case not handled in toggleLCDbacklight()
                }
                else if(numChars == 0) {
                    insertUARTtxBuffer("\r\nNo value entered\r\n\0");
                    break;
                }
                else {
                    insertUARTtxBuffer("\r\nValue \0");
                    insertUARTtxBuffer(LCDb);
                    insertUARTtxBuffer(" not OK, try again\r\n>\0");
                    LCDb[1]=0;LCDb[2]=0;LCDb[3]=0; numChars=0;
                }
            }
            else if(uartChar == 'q')  {
                testString[0] = uartChar;
                insertUARTtxBuffer(testString);
               break;
            }
        }
    }
}
