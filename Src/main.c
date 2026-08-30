
#include <stdint.h>
#include <midiCon.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define NUMBER_OF_REGISTERS 2
volatile uint8_t spi_buffer[NUMBER_OF_REGISTERS];

void init(void) {
	// allow clock for all used peripherals
	RCC->AHB1ENR |= (1U << 0);	// for GPIOA
	RCC->APB1ENR |= (1U << 0);	// for TIM2
	RCC->APB2ENR |= (1U << 12);	// for SPI1

	// GPIO configuration
	// PA2 = triggers SH/LD on external shift register
	GPIOA->MODER &= ~(3U << 4);
	GPIOA->MODER |= (1U << 4);	// out (01)
	GPIOA->OSPEEDR |= (2U << 4);
	GPIOA->BSRRL = (1U <<2);	// default logical 1 (SH/LD HIGH)
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
	GPIOA->OSPEEDR |= (2U << 12);
	GPIOA->PUPDR &= ~(3U << 12);
	GPIOA->PUPDR |= (1U << 12);	// pull-up

	// SPI1 configuration
	SPI1->CR1 = 0;					// reset of CR1, all bits to 0
	SPI1->CR1 |= (1U << 2);			// select master mode
	SPI1->CR1 |= (3U << 3);			// baud rate 84MHz / 16
	SPI1->CR1 |= (1U << 9);			// NSS directed by software
	SPI1->CR1 |= (1U << 8);			// sets this bit to 1, the value of physical NSS pin is ignored (NSS = negative slave select, but my external shift register doesn't need it)
	SPI1->CR1 &= ~(1U << 11);		// 0 is for 8-bit data
	SPI1->CR1 |= (1U << 6);			// enables SPI

	// TIM2 configuration
	TIM2->PSC = 8399;				// 84MHz / psc = 10kHz
	TIM2->ARR = 999;				// 10kHz / arr = 10Hz, 100ms
	TIM2->DIER |= (1U << 0);		// update event can start interrupt
	NVIC->ISER |= (1U << 28);		// specific interrupt (28)
	TIM2->CR1 |= (1U << 0);			// start TIM2


}

void TIM2_IRQHandler(void) {
	if (TIM2->CR1 & (1U << 0)) {	// to make sure timer is running
		if (TIM2->DIER & (1U << 0)) {	// interrupt is active
			TIM2->SR &= ~(1U << 0);	// eliminate interrupt flag
			GPIOA->BSRRH = (1U << 2);	// PA2 to LOW
			for (volatile int d=0; d<20; d++);
			GPIOA->BSRRL = (1U << 2);	// PA2 tO HIGH

			for (int i= 0; i < NUMBER_OF_REGISTERS; i++) {
				volatile uint32_t clear_flags = SPI1->DR;
				clear_flags = SPI1->SR;
				SPI1->DR = 0x00;		// starts clock at PA5
				while(!(SPI1->SR & (1U << 0)));	// no more bits in queue
				spi_buffer[i] = (uint8_t)SPI1->DR;
			}
		}
	}
}

int main(void)
{
	init();

	while (1) {
		uint8_t current_data0 = spi_buffer[0];
		uint8_t current_data1 = spi_buffer[1];
		for (volatile uint8_t j; j<5; j++);
		// use the data
	}


}
