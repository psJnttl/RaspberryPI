#ifndef PDC8544_H
#define PDC8544_H
#include "port.h" 

#define FUNCTION_SET        0x20
#define DISPLAY_CONTROL     0x08
#define SET_Y_ADDRESS       0x40
#define SET_X_ADDRESS       0x80
#define TEMPERATURE_CONTROL 0x04
#define BIAS_SYSTEM         0x10
#define SET_VOP             0x80

#define USE_EXTENDED_INSTRUCTION_SET  0x01
#define DISPLAY_BLANK       0x00
#define DISPLAY_ALL_SEG_ON  0x01
#define DISPLAY_NORMAL      0x04
#define DISPLAY_INVERSE     0x05
#define LCD_COMMAND         0
#define LCD_DATA            1

#define CHAR_SIZE    8
#define LINE_LENGTH  10
#define LINE_BUFFER_SIZE (LINE_LENGTH * CHAR_SIZE)
#define MESSAGE_MAX_LEN 101 // + terminating '\0'
extern void initScroller(char *msg, int line);
extern void updateScrollBuffer(void);
extern void operateScroll(void);

extern void pdc8544_init(void);
extern void printToPDC8544(const char *str, byte line);
extern void ClearScreen(void);
extern void SetLine(unsigned int line);
extern void SetColumn(unsigned int column);
extern void printCharToPDC8544(byte c);
extern void writeLongToPDC8544asDec(unsigned long c);
extern void printStringToPDC8544(const char *ptr); // asciiz
extern void initScroller(char *msg, int line);
extern void getScrollMsg(void);
#endif /* PDC8544_H */
