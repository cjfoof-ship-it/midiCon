#midiCon - shift register reader (SN74HC165N)

This subproject reads digital inputs from buttons via 8-bit parallel shift register **SN74HC165N** and writes them into variable on **STM32f407g**.

# architecture
reading takes place in the background, it does not block the main loop
 * hardware timer **TIM2** has period of 100ms (10 Hz)
 * every overload calls interrupt TIM2_IRQHandler
 * this interrupt generates Latch pulse, starts **SPI1** clock and reads 8 bits into global variable spi_buffer

# pinout


# gotchas
 * on shift register, Clock Inhibit (pin 15) has to be connected to **GND**, otherwise internal clock does not work properly and MISO reads static value
 * pin PA6 is configured with **internal pull-up** --> if disconnected, it gives stable 1

# read the data
To read the data, access global variable **spi_buffer** in main loop

# roadmap
To continue with this project, these are the following steps:
* [] **Amplify the hardware - Daisy chaining** to add more control buttons. When using 2 shift registers, change SPI to read 2 bytes.
* [] **Data parsing** to transfer bits into concrete readable values that describes the actual state of the buttons
* [] **Send data via UART**, implement periphery USART/UART to send data into PC/module
* [] **MIDI protocol**, formatting data into standard MIDI messages for implementation of midiCon as a MIDI controller 
