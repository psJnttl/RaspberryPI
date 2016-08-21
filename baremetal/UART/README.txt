I haven't seen too many of these around, just miniUART ones and usually in blocking mode ie. without interrupts. So here's UART  communication on Raspberry Pi v2B with interrupts. Had to disable UART FIFOs. Otherwise typing will feel a bit choppy because minimum 
trigger level for FIFO is two bytes. This should be OK since I implemented ringbuffer for both TX and RX.

To test the UART functionality there's main menu and the one for entering current time. And the obligatory LED blinking.
------------------------------
 1 - Set time
 2 - Get time now
 3 - Toggle OK LED
Typing is echoed back. Pressing enter won't feed new line but just carriage return. Free running system timer counter (CLO) register 
is used to keep time.



