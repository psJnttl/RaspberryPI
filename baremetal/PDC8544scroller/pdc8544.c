#include "port.h"
#include "gpio.h"
#include "pdc8544.h"
#include "fontdata.h"
#include "spi.h"
#include "interrupts.h"

static byte Line=0, Column=0;

void SetLine ( unsigned int line ) {
    if( line >= 0 && line <= 5) {
        SPIcommand(SET_Y_ADDRESS | line );
    }
    Line = line;
}

void SetColumn ( unsigned int column ) {
    if(column >= 0 && column <= 83) {
        SPIcommand(SET_X_ADDRESS | column);
    }
    Column = column;
}

void ClearScreen( void ) {
    unsigned int ch=0;
    SetLine(0); Line = 0;
    SetColumn(0); Column = 0;
    for(ch = 0; ch < 504; ch++) {   // 6 lines  x 84 columns
        SPIdata(0);
    }
}

void printToPDC8544(const char *str, byte line) { // line holds 10 letters
    unsigned char i=0, j=0, ch=0, accumulated=0;
    if(line >=0 && line <= 5) {
        SetLine(line); SetColumn(0);
        for(i = 0; i < 10; i++) {  // go through the string
            ch = str[i];
            if(ch == 0) {  // nonsense after \0
                break;
            }
            for(j = 0;j < 8;j++) { // letter consists of 8 bytes
                SPIdata(fontdata[ch][j]);
                accumulated++;
            }
        }
        for(;accumulated<84;accumulated++) SPIdata(0);
    }
    else {
        ClearScreen();
        printToPDC8544("  ERROR", 1); // yes, recursion
    }
}
void printCharToPDC8544(byte c) {
    byte i;
    for(i = 0;i < 8;i++) { // letter consists of 8 bytes
        SPIdata(fontdata[c][i]);
    }
}

void printStringToPDC8544(const char *ptr) { // asciiz
    int i=0;
    while(1) {
        if(ptr[i] == '\0' || i >= 10) break;
        printCharToPDC8544(ptr[i++]);
    }
}

void writeLongToPDC8544asDec(unsigned long c) {
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
    for(j=0; i<10; i++,j++) {  // j condition obsolete
        result[j] = result[i];
    }
    if(i==10 && j==0) {  // it's zero, let's print it too.
        result[0] = 0x30;
        j=1;
    }
    result[j] = '\0';
    for(i=0; i < j; i++) {
        printCharToPDC8544(result[i]);
    }
}

void writeLongToPDC8544asHex(unsigned long c) {
    char result[11];
    int shft=0, chr=0, i=0;
    for(shft = 28, i=0; shft >= 0; shft-=4, i++) {
        chr = ((c >> shft) & 0xF);
        if(chr >= 0 && chr <= 9) {
            chr += 0x30;
        }
        else {
            chr += 0x37;
        }
        result[i] = chr;
    }
    for(i = 0; i < 8; i++) {  // prefixing 0 removal
        if(result[i] != '0') break; // i marks the spot
    }
    for(shft=0; shft < 8, i < 8; shft++,i++) { // [8]=='\0'
        result[shft] = result[i];
    }
    result[shft] = '\0'; // needed for e.g. printf
    if(c == 0) {
        result[0] = 0x30; result[1] = '\0';
        shft++;  // no use with printStringToPDC8544()
    }
    printStringToPDC8544(result);
}

void pdc8544_init(void) {
    hword i;
    writeToGPIOpin(7, 1); // RST signal up
    for(i=0; i < 10000; i++);  // delay
    writeToGPIOpin(7, 0);  // RST signal down
    for(i = 0; i < 10000; i++);  // delay, tRW minumum value not defined
    writeToGPIOpin(7, 1); // RST signal up again
//Extended mode needed for bias, temperature control and Vop
    SPIcommand(FUNCTION_SET | USE_EXTENDED_INSTRUCTION_SET );  // 0x20 | 0x01   H = 1
// Not in example, 0x10 | 0x04 <-  recommended value (4) in PCD8544 Data Sheet  section 8.8 Bias value
    SPIcommand(BIAS_SYSTEM | 0x04);
    SPIcommand(SET_VOP | 0x3f); //  3.06V + 0x7F x 0.06V = 10,68V  i.e max. ; 3.06V + 0x3F x 0.06V = 6.84V
    SPIcommand(TEMPERATURE_CONTROL | 0x00); //temperature coefficient
    SPIcommand(FUNCTION_SET);  // 0x20 | PD=0 chip is active, V=0 horizontal addressing, H=0 basic intruction set
    SPIcommand(DISPLAY_CONTROL | DISPLAY_NORMAL);  // 0x08 | 0x04 (D=1, E=0) display to normal mode
    ClearScreen();
//    printToPDC8544("PDC 8544  ", 1); // line holds 10 letters
//    printToPDC8544("LCD ready.", 2);
}

/* ---------------------------------------------------------------------------
    Scroller section
   --------------------------------------------------------------------------- */
static char message [MESSAGE_MAX_LEN];
static char lineBuffer[LINE_BUFFER_SIZE];   // for PDC8544 line
static int msgIndex = 0; // keeps track of the current letter of message
static int charIndex = 0; // which byte of letter's fontdata
static int lineOnLCD = 0; // 0-5
static int messageLength = 0;
/* --------------------------------------------------------------------------- */

void resetScrollerMessage(void) {
    int i;
    for(i = 0; i < MESSAGE_MAX_LEN; i++ ) {
        message[i] = 0;
    }
}
void setScrollerMessage(char *msg) { // asciiz
    int i;
    resetScrollerMessage();
    messageLength = MESSAGE_MAX_LEN -1;
    for(i = 0; i < MESSAGE_MAX_LEN; i++ ) {
        if(msg[i] == '\0') {
            messageLength = i;
/*            printToPDC8544("msgLength", 1);
            SetLine(2); SetColumn(0);
            writeLongToPDC8544asHex(messageLength);
*/            break;
        }
        message[i] = msg[i];
    }
    msgIndex = 0;
    charIndex = 0;
}

void initLineBuffer(void) {
    int i=0;
    for (i = 0; i < LINE_BUFFER_SIZE; i++ ) {
        lineBuffer[i] = 0;
    }
}

void initScroller(char *msg, int line) { // ptr to message to be shown, line on LCD
    initLineBuffer();
    setScrollerMessage(msg);
    if (line >= 0 && line <= 5) {
        lineOnLCD = line;
    }

}

void updateScrollBuffer(void) { // run after operateScroll() in normal mode
    int i=0, scrollAmount = LINE_BUFFER_SIZE-1;
// shift line buffer to left one byte
    for (i = 0; i < scrollAmount; i++ ) {
        lineBuffer[i] = lineBuffer[i+1];
    }
// get next byte of the next letter from fontdata
    lineBuffer[LINE_BUFFER_SIZE-1] = fontdata[message[msgIndex]][charIndex++];
    if (charIndex > 7) {
        msgIndex++;
        charIndex = 0;
    }
    if(msgIndex > messageLength) {
        msgIndex = 0;
    }
}

void operateScroll(void) {  // could be called from interrupt, signal updateScrollBuffer() with semaphore
    int i=0;
    SPIcommand(SET_Y_ADDRESS | lineOnLCD );
    SPIcommand(SET_X_ADDRESS | 0);
    for(i = 0; i < LINE_BUFFER_SIZE; i++) {
        SPIdata(lineBuffer[i]);
    }
}

void getScrollMsg(void) {
    unsigned char string[9]= {0,0,0,0,0,0,0,0,0}, uartChar=0;
    char tmpBuf [MESSAGE_MAX_LEN];
    int i=0;
    insertUARTtxBuffer("\r\n----------------------------\r\n\0");
    insertUARTtxBuffer("Please enter message, max. length 100 letters.\r\n\0");
    while(1) {
        if(uartI == 0x80) {   // UART RX
            uartChar = readCharFromUARTrxBuffer();
            if(uartChar != 0) {
                string[0] = uartChar;
                if (uartChar >= 32 && uartChar <= 127)  insertUARTtxBuffer(string);     // echo it right back

                if(i > MESSAGE_MAX_LEN) {
                    tmpBuf[i-1] = '\0';
                    setScrollerMessage(tmpBuf);
                    insertUARTtxBuffer("\r\nMaximum length exceeded, message set.\r\n\0");
                    usWait(2000000); // 2s
                    return;
                }
                else if (uartChar >= 32 && uartChar <= 126)  {
                    tmpBuf[i++] = uartChar;
                }
                else if(uartChar == 13) {
                    tmpBuf[i] = '\0';
                    setScrollerMessage(tmpBuf);
                    insertUARTtxBuffer("\r\nMessage has been set.\r\n\0");
                    return;
                }
                else if(uartChar == 127) {
                    if ( i > 0) tmpBuf[--i] = ' ';
                }
            }
        uartI &= ~0x80;
        }        
    }
}

/* --------------------------------------------------------------------------- */
