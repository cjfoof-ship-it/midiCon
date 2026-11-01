# this is README for DEBUG VERSION of midiCon project

#midiCon - shift register reader (SN74HC165N)

This project reads digital inputs from buttons via 8-bit parallel shift register **SN74HC165N** and writes them into variable on **STM32f407g**. Later on, these data are processed and turned into MIDI commands.

# KNOWN ISSUES
* **UART Data Corruption:** The UART peripheral transmits corrupted data and requires further debugging. The issue is suspected to be caused by either internal conflicts within the USART peripheral interrupts or hardware instability on pin PA9 due to internal MCU multiplexing and cross-connections.

# Architecture Updates & Test Configurations

### 1. Increased Polling Rate (TIM2 Math Fix)
The system clock is running at **16 MHz**. 
* **TIM2** has been re-tuned with a lower prescaler (`TIM2->PSC = 159`) and an auto-reload target of `TIM2->ARR = 999`. 
* **Result:** This successfully accelerates the main polling loop to a precise **10 ms interval (100 Hz)**, providing highly responsive button tracking that captures immediate taps.

### 2. Thread Safety (`new_data_ready` flag)
To resolve asynchronous data racing between the background `TIM2_IRQHandler` and the `main()` while-loop, a global thread gatekeeper has been added:

volatile uint8_t new_data_ready = 0;

This flag stops the `main()` loop from executing mid-read when the background SPI transfer modifies data buffers, preventing partial byte corruption.

### 3. Bit-Banged Test Diagnostics (TIM5 & GPIOA 7)
To test processing overhead, data transmission behavior, and timing limits:
* **TIM5 Configuration:** Configured with `PSC = 8` and `ARR = 56` to create an isolated hardware clock. On our 16 MHz bus, this executes at approximately **31.2 kHz**, aligning tightly with standard 31,250 baud MIDI serial specs.
* **Pin PA7 (GPIO Toggle Hook):** Pin `PA7` has been configured as a high-speed digital output with an active pull-up. This allows us to hook up a logic analyzer or oscilloscope to physically track execution time and verify the actual state of sent bytes.

# architecture
reading takes place in the background, it does not block the main loop
 * hardware timer **TIM2** has period of 100ms (10 Hz)
 * every overload calls interrupt TIM2_IRQHandler
 * this interrupt generates Latch pulse, starts **SPI1** clock and reads 8 bits into global variable spi_buffer

turning raw data into MIDI commands is done in **main** function and in function **sendTEST** (in this version)

# pinout
STM:  GND   -----   GND   (PIN 8)   :SN74HC165N  
      3V    -----   VCC   (PIN 16)  
      PA2   -----   SH/LD   (PIN 1)  
      PA5   -----   CLK   (PIN 2)  
      PA6   -----   QH   (PIN 9)  
                     CLK INH (PIN 15) to GND  
                     PINS 3,4,5,6,11,12,13,14 to buttons (inputs)  
                     when connected in daisy-chain, connect shift registers: SER (PIN 10) to QH (PIN 9)  
                     
To send data out as MIDI message, pin PA7 is used in this version.
                     

# gotchas
 * on shift register, Clock Inhibit (pin 15) has to be connected to **GND**, otherwise internal clock does not work properly and MISO reads static value
 * pin PA6 is configured with **internal pull-up** --> if disconnected, it gives stable 1
 * **Decoupling Capacitor (100nF):** Placed as close as possible between **Pin 16 (VCC)** and **Pin 8 (GND)** of the SN74HC165N. This suppresses voltage drops and noise caused by high-speed SPI clock switching.
 *  **Button Pull-Up Resistors (10kΩ):** The circuit implements configuration where buttons switch to **GND**. Each digital input pin (D0–D7) of the shift register is connected to **3.3V (VCC)** via a 10kΩ pull-up resistor to maintain a stable logic `1` when the button is open. Pressing a button pulls the corresponding input pin directly to **GND**, creating a logic `0`. This matches the internal pull-up configuration on the STM32 `PA6` (MISO) line, ensuring consistent active-low signal logic across the system. To prevent signal noise and debouncing issues, a **100nF capacitor** and a **1kΩ resistor** are added to each button. This forms a simple RC low-pass filter that suppresses contact bounce and protects the inputs against sudden voltage transients during rapid switching.

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
* [X] **test with logic analyzer**
