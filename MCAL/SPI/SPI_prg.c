/*
 * SPI_prg.c
 *
 *  Created on: Jul 31, 2025
 *      Author: acer
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../DIO/DIO_int.h"
#include "SPI_int.h"
#include "SPI_prv.h"
#include "SPI_cfg.h"

void MSPI_vInit(void) {
#if SPI_MODE == SPI_MASTER

	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN5, DIO_OUTPUT);
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN7, DIO_OUTPUT);
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN6, DIO_INPUT);
#endif
#if SPI_MODE == SPI_SLAVE

	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN5, DIO_INPUT);
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN7, DIO_INPUT);
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN6, DIO_OUTPUT);
#endif
	SPCR = 0b01011010;
}
void MSPI_vTransmit(u8 A_u8Byte) {
	SPDR = A_u8Byte;
	while (!(SPSR & (1 << SPIF)))
		;

}

u8 MSPI_vTransceive(u8 A_u8Byte) {
	SPDR = A_u8Byte;
	while (!(SPSR & (1 << SPIF)))
		;
	return SPDR;

}
u8 MSPI_u8Receive(void) {
	while (!GET_BIT(SPSR, SPIF))
		;
	return SPDR;

}
