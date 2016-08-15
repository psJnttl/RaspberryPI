I haven't seen too many of these around, just miniUART ones and usually in blocking mode ie. without interrupts. So here's UART  communication on Raspberry Pi v2B with interrupts. Had to disable UART FIFOs. Otherwise typing will feel a bit choppy because minimum 
trigger level for FIFO is two bytes. This should be OK since I implemented ringbuffer for both TX and RX.

To test the UART functionality there's main menu and the one for entering current time. And the obligatory LED blinking.
------------------------------
 1 - Set time
 2 - Get time now
 3 - Toggle OK LED
Typing is echoed back. Pressing enter won't feed new line but just carriage return. Free running system timer counter (CLO) register 
is used to keep time.

If you're just starting bare metal coding on Raspberry Pi don't even think about it before taking a bootloader into use. One can be 
found at https://github.com/dwelch67/raspberrypi. Basically replace the kernel IMG you normally do when testing your project build. 
Bootloader will operate on higher memory address and thus can load your build to normal address range.

No more changing SD card between development PC (or another RPi) and Raspberry Pi. Just compile the project in hex format and upload 
it to Raspberry Pi Under Development (RPUD). I use cmake and project is compiled into regular IMG but also HEX. On PC end a terminal 
SW is needed, e.g. PuTTY for updloading. Some hardware also, a USB-UART interface: google: "FTDI USB UART raspberry pi".


