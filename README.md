#midiCon - shift register reader (SN74HC165N)

This project reads digital inputs from buttons via 8-bit parallel shift register **SN74HC165N** and writes them into variable on **STM32f407g**. Later on, these data are processed and turned into MIDI commands.

# architecture
reading takes place in the background, it does not block the main loop
 * hardware timer **TIM2** has period of 100ms (10 Hz)
 * every overload calls interrupt TIM2_IRQHandler
 * this interrupt generates Latch pulse, starts **SPI1** clock and reads 8 bits into global variable spi_buffer

turning raw data into MIDI commands is done in **main** function and in function **send_signal**

# pinout
STM:  GND   -----   GND   (PIN 8)   :SN74HC165N  
      3V    -----   VCC   (PIN 16)  
      PA2   -----   SH/LD   (PIN 1)  
      PA5   -----   CLK   (PIN 2)  
      PA6   -----   QH   (PIN 9)  
                     CLK INH (PIN 15) to GND  
                     PINS 3,4,5,6,11,12,13,14 to buttons (inputs)  
                     when connected in daisy-chain, connect shift registers: SER (PIN 10) to QH (PIN 9)  

# gotchas
 * on shift register, Clock Inhibit (pin 15) has to be connected to **GND**, otherwise internal clock does not work properly and MISO reads static value
 * pin PA6 is configured with **internal pull-up** --> if disconnected, it gives stable 1
 * **Decoupling Capacitor (100nF):** Placed as close as possible between **Pin 16 (VCC)** and **Pin 8 (GND)** of the SN74HC165N. This suppresses voltage drops and noise caused by high-speed SPI clock switching.
 *  **Button Pull-Up Resistors (10kΩ):** The circuit implements configuration where buttons switch to **GND**. Each digital input pin (D0–D7) of the shift register is connected to **3.3V (VCC)** via a 10kΩ pull-up resistor to maintain a stable logic `1` when the button is open. Pressing a button pulls the corresponding input pin directly to **GND**, creating a logic `0`. This matches the internal pull-up configuration on the STM32 `PA6` (MISO) line, ensuring consistent active-low signal logic across the system.

# file locations
The core logic and configuration are here:
 * Src/main.c
 * Inc/midiCon.h  - custom peripheral structures and memory base addresses
   
# read the data
To read the data, access global variable **spi_buffer** in main loop. To read actual MIDI commands to be send, read variables in function 'send_signal()' or you can see it as arguments in function 'message()'

# roadmap
To continue with this project, these are the following steps:
* [x] **Amplify the hardware - Daisy chaining** to add more control buttons (DONE, now set to 2 shift registers, but can be adjusted in variable NUMBER_OF_REGISTERS)
* [X] **Data parsing** to transfer bits into concrete readable values that describes the actual state of the buttons
* [X] **Send data via UART**, implement periphery USART/UART to send data into PC/module
* [X] **MIDI protocol**, formatting data into standard MIDI messages for implementation of midiCon as a MIDI controller
* [ ] **test with logic analyzer**
