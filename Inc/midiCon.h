/*
 * midiCon.h
 *
 *  Created on: Aug 24, 2026
 *      Author: linuxlite
 */

#ifndef MIDICON_H_
#define MIDICON_H_

#include <stdint.h>

//RCC
typedef struct {
	volatile uint32_t	BASE;	// 0x00
	uint32_t			DUMMY[11];
	volatile uint32_t	AHB1ENR;	// 0x30
	uint32_t			DUMMY2[3];
	volatile uint32_t	APB1ENR;	//0x40
	volatile uint32_t	APB2ENR;	//0x44
} RCC_TypeDef;

//GPIO
typedef struct {
	volatile uint32_t	MODER;	//0x00
	uint32_t			DUMMY;
	volatile uint32_t	OSPEEDR;	//0x08
	volatile uint32_t	PUPDR;		//0x0C
	uint32_t			DUMMY2[2];
	volatile uint16_t	BSSRL;	// low 16 bits to set on 0x18
	volatile uint16_t	BSSRH;	// high 16 bits to reset on 0x18
	uint32_t			DUMMY3;
	volatile uint32_t	AFRL;	//0x20


} GPIO_TypeDef;

//TIM
typedef struct {
	volatile uint32_t	CR1;	//0x00
	volatile uint32_t	CR2;	//0x04
	uint32_t			DUMMY[5];
	volatile uint32_t	CCMR2;	//0x1C
	volatile uint32_t	CCER;	//0x20
	uint32_t			DUMMY2;
	volatile uint32_t	PSC;	//0x28
	volatile uint32_t	ARR;	//0x2C
	uint32_t			DUMMY3[3];
	volatile uint32_t	CCR3;	//0x3C
} TIM_TypeDef;

// SPI
typedef struct {
	volatile uint32_t	CR1;	//0x00
	volatile uint32_t	CR2;	//0x04
	volatile uint32_t	SR;		//0x08
	volatile uint32_t	DR;		//0x0C
} SPI_TypeDef;

//DMAStream		dma2 stream 0 channel 3 for SPI1_RX
typedef struct {
	volatile uint32_t	CR;		//0x00
	volatile uint32_t	NDTR;	//0x04
	volatile uint32_t	PAR;	//0x08
	volatile uint32_t	M0AR;	//0x0C
} DMAStream_TypeDef;


// base addresses
#define RCC		((RCC_TypeDef *) 0x40023800UL)
#define GPIOA	((GPIO_TypeDef *) 0x40020000UL)
#define TIM2	((TIM_TypeDef *) 0x40000000UL)
#define SPI1	((SPI_TypeDef *) 0x40013000UL)
#define DMA2_Stream0	((DMAStream_TypeDef *) 0x40026410UL)


#endif /* MIDICON_H_ */
