# STM32 Half Duplex Boot Loader
Second stage bootloader for STM32G0x series with YMODEM programming over UART. 

This project was made as part of another project, requiring the ability to update the program code over the power supply lines only.

The code the minimum required to work and does not do any ckecking of the flash before boot or enable write protection on itself.

## Usage
- Open with platformIO and compile! You will likely need to modify the UART setup in ```uart.c```. 
- Run the YMODEM transmission program on a PC and reset the MCU. On boot, the MCU should initiate the transmission.
- When the transfer is comlpete or the MCU does not receive a file, the bootloader boots from ```0x8008000```.

The loaded program should be linked so the flash start address is ```0x8008000```.
