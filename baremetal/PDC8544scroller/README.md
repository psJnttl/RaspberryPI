PDC8544 is a graphic LCD used in old Nokia phones. Monochrome, low cost and good availability. Some soldering required but 
some vendors have soldering service so once you get the display it's got all the wires needed.

After some data sheet studying and googling the internet to see what others have already done it was relatively easy to 
implement the basic functionality, ie. printing text to desired line on the LCD. PDC8544 uses SPI which is convenient in 
Raspberry Pi use. Font data is from https://github.com/dwelch67. Look under raspberrypi/spi02. Scrolling was simple to 
implement since data goes to LCD column at a time.

