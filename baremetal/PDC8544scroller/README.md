PDC8544 is a graphic LCD used in old Nokia phones. Monochrome, low cost and good availability. Some soldering required but 
some vendors have soldering service so once you get the display it's got all the wires needed.

After some data sheet studying and googling the internet to see what others have already done it was relatively easy to 
implement the basic functionality, ie. printing text to desired line on the LCD. PDC8544 uses SPI which is convenient in 
Raspberry Pi use. Font data is from https://github.com/dwelch67. Look under raspberrypi/spi02. Scrolling was simple to 
implement since data goes to LCD column at a time.

I hooked the PWM to control the backlight. This way it's possible to control backlight brightness from code. In my code sample 
duty cycle default is 10% which you might think sounds a bit dim but is quite enough. PWM range register has been 
configured to enable brightness setting from 0 to 100 from terminal SW menu.

This is a pretty good site for PIN arrangement on RPi: http://pinout.xyz/

BCM pin      Function
7             output <-> PDC8544 RST
8             SPI0 Chip Enable <-> PDC8544 /SCE
9             SPI0 MISO - NOT CONNECTED to PDC8544 since data is one way
10            SPI0 MOSI <-> PDC 8544 SDIN
11            SPI0 CLK <-> PDC 8544 SCLK
18            PWM0 <-> PDC8544 LED
25            output <-> PDC 8544 data/command select
14            UART TX
15            UART RX

GPIO pin
1             3.3V <-> PDC8544 VCC
9             GND <-> PDC8544 GND
6             GND <-> UART ground
