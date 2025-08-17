/*
 * SPI_prv.h
 *
 *  Created on: Jul 31, 2025
 *      Author: acer
 */

#ifndef MCAL_SPI_SPI_PRV_H_
#define MCAL_SPI_SPI_PRV_H_

#define SPDR 	*((volatile u8*) (0x2F))
#define SPSR 	*((volatile u8*) (0x2E))
#define SPCR 	*((volatile u8*) (0x2D))


/* ---SPI Config--- */
#define SPI_MASTER		0
#define SPI_SLAVE		1

#define SPIF 			7
#endif /* MCAL_SPI_SPI_PRV_H_ */
