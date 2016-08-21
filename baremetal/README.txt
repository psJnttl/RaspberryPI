Folder for bare metal projects.

If you're just starting bare metal coding on Raspberry Pi don't even think about it before taking a bootloader into use. One can be 
found at https://github.com/dwelch67/raspberrypi. Basically replace the kernel IMG you normally do when testing your project build. 
Bootloader will operate on higher memory address and thus can load your build to normal address range.

No more changing SD card between development PC (or another RPi) and Raspberry Pi. Just compile the project in hex format and upload 
it to Raspberry Pi Under Development (RPUD). I use cmake and project is compiled into regular IMG but also HEX. On PC end a terminal 
SW is needed, e.g. PuTTY for updloading. Some hardware also, a USB-UART interface: google: "FTDI USB UART raspberry pi".
