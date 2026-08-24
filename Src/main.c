
#include <stdint.h>
#include <midiCon.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


volatile uint8_t spi_buffer = 0;

void init(void) {
	// allow clock for all used peripherals
	RCC->AHB1ENR |= (1U << 0);	// for GPIOA
	RCC->AHB1ENR |= (1U << 22);	// for DMA2
	RCC->APB1ENR |= (1U << 0);	// for TIM2
	RCC->APB2ENR |= (1U << 12);	// for SPI1

	// GPIO configuration
	// PA2 = triggers SH/LD on external shift register
	GPIOA->MODER &= ~(3U << 4);	//reset
	GPIOA->MODER |= (2U << 4); //sets AF for PA2
	GPIOA->AFRL &= ~(0xFU << 8);	// reset
	GPIOA->AFRL |= (2U << 8);	//sets AF2 for TIM2_CH3
	// PA5 = SPI1_SCK, triggers CLK on external shift register
	GPIOA->MODER &= ~(3U << 10);	//resets
	GPIOA->MODER |= (2U << 10);	//sets AF for PA5
	GPIOA->AFRL &= ~(0xFU << 20);	//resets
	GPIOA->AFRL |= (5U << 20);		// sets AF5 for SPI1_SCK
	GPIOA->OSPEEDR &= ~(3U << 10);
	GPIOA->OSPEEDR |= (2U << 10);	// high speed
	// PA6 for SPI1_MISO, data from external shift register enter
	GPIOA->MODER &= ~(3U << 12);
	GPIOA->MODER |= (2U << 12);	// sets PA6 to AF
	GPIOA->AFRL &= ~(0xFU << 24);
	GPIOA->AFRL |= (5U << 24);		// AF5 = SPI1_MISO
	GPIOA->PUPDR &= ~(3U << 12);
	GPIOA->PUPDR |= (1U << 12);	// pull-up

	// SPI1 configuration
	SPI1->CR1 = 0;					// reset of CR1, all bits to 0
	SPI1->CR1 |= (1U << 2);			// select master mode
	SPI1->CR1 |= (3U << 3);			// baud rate 84MHz / 16
	SPI1->CR1 |= (1U << 9);			// NSS directed by software
	SPI1->CR1 |= (1U << 8);			// sets this bit to 1, the value of physical NSS pin is ignored (NSS = negative slave select, but my external shift register doesn't need it)
	SPI1->CR1 &= ~(1U << 11);		// 0 is for 8-bit data
	SPI1->CR2 |= (1U << 0);			// Rx buffer DMA enable, received data will be available for direct memory access
	SPI1->CR1 |= (1U << 6);			// enables SPI

	// TIM2 configuration
	TIM2->PSC = 8399;				// 84MHz / psc = 10kHz
	TIM2->ARR = 999;				// 10kHz / arr = 10Hz, 100ms
	TIM2->CCMR2 &= ~(7U << 8);
	TIM2->CCMR2 |= (6U << 8);		// 110 = PWM mode 1 for channel 3
	TIM2->CCMR2 |= (1U << 3);		// output compare 3 preload enable, to store data first in preload register, making the reading more stable
	TIM2->CCR3 = 10;				// pulse width, 10 tics, ~1micro sec
	TIM2->CCER |= (1U << 8);		// capture/compare 3 output enable
	TIM2->CR2 &= ~(7U << 4);
	TIM2->CR2 |= (2U << 4);			// 10 = update event (for TRGO so it can trigger DMA)
	TIM2->CR1 |= (1U << 7);			// auto reload-preload, waits until the end of the period before changing the ARR value, adds more stability
	TIM2->CR1 |= (1U << 0);			// start TIM2

	// DMA2 configuration for Stream 0
	DMA2_Stream0->CR &= ~(1 << 0);	// resets
	while (DMA2_Stream0->CR & 1); 	// wait for it turns off
	DMA2_Stream0->PAR = (uint32_t)&SPI1->DR;	// source = SPI data register
	DMA2_Stream0->M0AR = (uint32_t)&spi_buffer;	// destination = spi_buffer
	DMA2_Stream0->NDTR = 1;			// transmit 1 byte
	DMA2_Stream0->CR = 0;
	DMA2_Stream0->CR |= (3U << 25);	// channel 3 for SPI1_RX
	DMA2_Stream0->CR |= (1U << 16);		// priority medium
	DMA2_Stream0->CR &= ~(3U << 11);	// peripheral data size = 8 bit
	DMA2_Stream0->CR &= ~(3U << 13);	// memory size 8 bit
	DMA2_Stream0->CR |= (1U << 10);		// memory increment (even though it make no difference for 1 byte
	DMA2_Stream0->CR |= (1U << 8);		// enable circular mode
	DMA2_Stream0->CR |= (1U << 0);		//enable DMA2_Stream0

}

int main(void)
{
	init();

	while (1) {
		uint8_t current_data = spi_buffer;
		// use the data
	}
}
