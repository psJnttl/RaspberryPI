#include "port.h"
#include "sysTimer.h"
#include <stdint.h>
#include "uart.h"
#include "interrupts.h"  // uartI
volatile word *sysTimeRegAddr = (word *)SYSTIMER_BASE;  // base pointer for registry access
static dword setTime64b; // system time when time was set. Need to be compared to global uptime in System Timer Counter
static dword timeNow64b, clockTimeBase64b, Seconds64b, work64b;
struct timeStr {
    byte hours;
    byte minutes;
    byte seconds;
};
static struct timeStr timeNowStr;

void initSysTime() {
    word regVal = sysTimeRegAddr[ST_CLO];
    return;
}

void getTimeNow(byte choice) {      // 64bit not OK in stack, RAM is better
    word rootTimeLS, rootTimeMS;    // 32bit tmp variables
    rootTimeLS = sysTimeRegAddr[ST_CLO];
    rootTimeMS = sysTimeRegAddr[ST_CHI];
    if(choice == 1) { // time now
        timeNow64b = (uint64_t)rootTimeMS;
        timeNow64b <<= 32;
        timeNow64b |= (uint64_t)rootTimeLS;
    }
    else if(choice == 2)  { // base time, 'when'
        setTime64b = (uint64_t)rootTimeMS;
        setTime64b <<= 32;
        setTime64b |= (uint64_t)rootTimeLS;
    }
}
void setTimeNow2baseTime() {
    work64b = timeNow64b - setTime64b; // new time from IRQ - old time stamp
    clockTimeBase64b += work64b;        // optimisation can be done, work64b superfluous
    if(clockTimeBase64b > 86400000000LL) clockTimeBase64b -= 86400000000LL; // day changed
    setTime64b = timeNow64b;
}

void calcClockTimeInUs() {  // "13:43:27" -> 64bit word, actually from timeNowStr
    clockTimeBase64b = (dword)timeNowStr.hours * 3600000000LL; // 23 * 3600 * 1000 000 = 82 800 000 000; 3600000000LL
    clockTimeBase64b += (dword)timeNowStr.minutes * 60000000LL; // 59* 60 * 1000 000 = 3 540 000 000    ; 60000000LL
    clockTimeBase64b += (dword)timeNowStr.seconds * 1000000LL;        //   1000000LL
} /* Tässä on paha 24h rajoitus */

void timeNowToPDC8544(byte line) {
    int Hours, Minutes, Seconds;
    Seconds64b = clockTimeBase64b / 1000000LL;
    Hours = Seconds64b / 3600;
    Minutes = (Seconds64b / 60 ) % 60;
    Seconds = Seconds64b % 60;
    SetLine(line);
    SetColumn(0);
    if(Hours < 10) printCharToPDC8544(0x30);
    writeLongToPDC8544asDec(Hours); printCharToPDC8544(':');
    if(Minutes < 10) printCharToPDC8544(0x30);
    writeLongToPDC8544asDec(Minutes); printCharToPDC8544(':');
    if(Seconds < 10) printCharToPDC8544(0x30);
    writeLongToPDC8544asDec(Seconds);
}

void timeNowToUART(byte lf) {
    int Hours, Minutes, Seconds;
    char buffer[9] = {0,0,0,0,0,0,0,0,0};
    Seconds64b = clockTimeBase64b / 1000000LL;
    Hours = Seconds64b / 3600;
    Minutes = (Seconds64b / 60 ) % 60;
    Seconds = Seconds64b % 60;
    if(Hours < 10) insertUARTtxBuffer("0\0");
    writeLongToUARTasDec(Hours); insertUARTtxBuffer(":\0");
    if(Minutes < 10) insertUARTtxBuffer("0\0");
    writeLongToUARTasDec(Minutes); insertUARTtxBuffer(":\0");
    if(Seconds < 10) insertUARTtxBuffer("0\0");
    writeLongToUARTasDec(Seconds);
    if(lf == 1) {
        insertUARTtxBuffer("\r\n\0");
    }
    else if(lf == 0) insertUARTtxBuffer("\n\0");
}

void printTimeMenu(void)  {
    insertUARTtxBuffer("\r\n----------------------------\r\n\0");
    insertUARTtxBuffer("Please enter time in format: HH:MM:SS\r\n\0");
}

void getTimeLoop(void) { // don't return anything because data remains in this module
    int numChars=0;
    char Hrs[3]={'0','0',0},  Mins[3]={'0','0',0},  Secs[3]={'0','0',0};
    unsigned char string[9]= {0,0,0,0,0,0,0,0,0}, uartChar=0;
    printTimeMenu();

    while(1) {
        if(uartI == 0x80) {   // UART RX
            uartChar = readCharFromUARTrxBuffer();
            if(uartChar != 0) {
                string[0] = uartChar;
                insertUARTtxBuffer(string);     // echo it right back
                if(uartChar >= '0' && uartChar <= '9') {
                    numChars++;
                    if(numChars == 1) Hrs[numChars-1] = uartChar;
                    else if(numChars == 2) {
                        Hrs[numChars-1] = uartChar;
                        if(atoi(Hrs) < 0 || atoi(Hrs) > 23) {
                            insertUARTtxBuffer("  Incorrect hours entered, try again.\r\n\0");
                            numChars = 0;
                        }
                        else insertUARTtxBuffer(":\0");
                    }
                    else if(numChars == 3) Mins[numChars-3] = uartChar;
                    else if(numChars == 4) {
                        Mins[numChars-3] = uartChar;
                        if(atoi(Mins) < 0 || atoi(Mins) > 59) {
                            insertUARTtxBuffer("  Incorrect minutes entered, try again.\r\n\0");
                            numChars = 2;
                            insertUARTtxBuffer(Hrs); insertUARTtxBuffer(":\0");
                            }
                        else insertUARTtxBuffer(":\0");
                    }
                    else if(numChars == 5) Secs[numChars-5] = uartChar;
                    else if(numChars == 6) {
                        Secs[numChars-5] = uartChar;
                        if(atoi(Secs) < 0 || atoi(Secs) > 59) {
                            insertUARTtxBuffer("  Incorrect seconds entered, try again.\r\n\0");
                            numChars = 4;
                            insertUARTtxBuffer(Hrs); insertUARTtxBuffer(":\0");
                            insertUARTtxBuffer(Mins); insertUARTtxBuffer(":\0");
                        }
                    }
                } //uartChar >= '0' && uartChar <= '9'
                else if(uartChar == 13) { // enter
                    getTimeNow(2);// no1: 'record' system time when clock time entered
                    // calculate clock time in us for further operations
                    timeNowStr.hours = atoi(Hrs);
                    timeNowStr.minutes = atoi(Mins);
                    timeNowStr.seconds = atoi(Secs);
                    calcClockTimeInUs(); // uses struct values, stores to static 64bit variable 'clockTimeBase64b'
                    // feedback to user
                    insertUARTtxBuffer("\r\nTime entered, clock is now: \0");
                    timeNowToUART(1);  // with LF
                    break;
                }
                else if(uartChar == 'q') { // 113,  back space on 127
                    insertUARTtxBuffer("\r\nQuit Set time\r\n\0");
                    break;
                }        
            } /* char != 0 */
            uartI = uartI & ~0x80;
        } /* UART RX */
    } /* while */
}

void usWait(word usecs) { //
    word now = sysTimeRegAddr[ST_CLO];
    sysTimeRegAddr[ST_CS] = 0x01; // reset C0
    sysTimeRegAddr[ST_C0] = usecs + now;
    while(1)  { // wait until system timer catches up
        if(sysTimeRegAddr[ST_CS] & 0x01) {
            break;
        }
    }
}
