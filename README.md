# STM32 Half Duplex Boot Loader
Second stage bootloader for STM32G0x series with YMODEM programming over UART. 

This project was made as part of another project, requiring the ability to update the program code over the power supply lines only.

The code the minimum required to work and does not do any ckecking of the flash before boot or enable write protection on itself.

## Usage
- Open with platformIO and compile! You will likely need to modify the UART setup in ```uart.c```. 
- Run the YMODEM transmission program on a PC and reset the MCU. On boot, the MCU should initiate the transmission.
- When the transfer is comlpete or the MCU does not receive a file, the bootloader boots from ```0x8008000```.

The loaded program should be linked so the flash start address is ```0x8008000```.

## Application
![image](https://github.com/user-attachments/assets/acf5fc23-22da-4aa9-bed9-e1a1b4cd9668)

The application consists of an STM32 - the power supplly is connected directly to a UART Tx pin, and internally connected to the Rx pin in software. A diode and capacitor provides a stable power supply from the half-duplex uart signal on Vbus. This keeps the STM32 powered while it communicates. When the communication is finished, the program can pull the PWR_EN pin low to connect VBus directly to the power supply, avoiding the diode forward voltage drop. It is also possible to not use the external diode and rely on the mosfet body diode, as long as the voltage drop is low enough that the microcontroller can still be powered.
