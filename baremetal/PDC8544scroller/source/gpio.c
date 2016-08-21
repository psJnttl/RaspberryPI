#include "port.h"
#include "gpio.h"

volatile word *regAddr = (word *)GPIO_BASE;  // base pointer for registry access

void configureGPIOpin(byte GPIOpin, byte mode) {  // use enum for mode
    word *regAddr = (word *)GPIO_BASE, GPSELoffset = (GPIOpin / 10), regVal=0;
    byte bitOffset = (GPIOpin % 10) * 3;  // which 3 bits in the register?
    regVal = regAddr[GPSELoffset];
    regVal &= ~((0x07) << bitOffset);     // reset first
    regVal |= (0x07 & mode) << bitOffset; // mode 0-7
    regAddr[GPSELoffset] = regVal;
}

void writeGPIOreg(byte GPIOreg, word data) { // this is pretty blunt instrument
    regAddr[GPIOreg] = data;
}

word readGPIOreg(byte GPIOreg) {
    return regAddr[GPIOreg];
}

void writeToGPIOpin(byte GPIOpin, byte data) {
    if(data) { // set PIN
        if(GPIOpin > 31) {
            regAddr[GPSET1] = 1 << (GPIOpin-32); // 0 does nothing, 1 sets the PIN
        }
        else {
            regAddr[GPSET0] = 1 << (GPIOpin);
        }
    }
    else { // clear pin
        if(GPIOpin > 31) {
            regAddr[GPCLR1] = 1 << (GPIOpin-32);
        }
        else {
            regAddr[GPCLR0] = 1 << (GPIOpin);
        }
    }
}

byte readFromGPIOpin(word GPIOpin) {
    word pinValue = 0xFF;
    if(GPIOpin > 31 && GPIOpin < 54) { // highest pins
        pinValue = regAddr[GPLEV1];
        pinValue = pinValue & (1<<(GPIOpin-32));  // zero all other output results
        pinValue = pinValue >> (GPIOpin-32);      // Shift pin value to LSB
    }
    else if (GPIOpin >= 0 && GPIOpin <= 31) { // lowest 32 pins
        pinValue = regAddr[GPLEV0];
        pinValue = pinValue & (1<<(GPIOpin)); 
        pinValue = pinValue >> GPIOpin;
    }
    return pinValue;
}
