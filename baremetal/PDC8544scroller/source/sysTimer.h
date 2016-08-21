#ifndef SYSTIMER_H
#define SYSTIMER_H

#define SYSTIMER_BASE       0x3F003000  // BCM2835-ARM-Perip  p172
enum {  // There's 7 System Timer registers
ST_CS,   // 0x00 System Timer Control/Status
ST_CLO,  // 0x04 System Timer Counter Lower 32 bits
ST_CHI,  // 0x08 System Timer Counter Higher 32 bits
ST_C0,   // 0x0c System Timer Compare 0
ST_C1,   // 0x10 System Timer Compare 1
ST_C2,   // 0x14 System Timer Compare 2
ST_C3    // 0x18 System Timer Compare 3
};

extern void getTimeNow(byte choice);
extern void getTimeLoop(void);
extern void timeNowToUART(byte lf);
extern void setTimeNow2baseTime();
extern void timeNowToPDC8544(byte line);
extern void usWait(word usecs);
#endif /* SYSTIMER_H */

