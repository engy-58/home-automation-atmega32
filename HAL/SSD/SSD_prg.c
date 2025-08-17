/*
 * SSD_prg.c
 *
 *  Created on: Jul 20, 2025
 *      Author: acer
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/DIO_int.h"
#include "SSD_int.h"

static u8 sevenSegment[] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111,
		0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111 };

void HSSD_vInit() {
	MDIO_vSetPortDir(SSD_PORT, 0xff);
}

void HSSD_vDisplayNumaber(u8 A_u8Num) {
	if (A_u8Num >= 0 && A_u8Num <= 9) {
#if SSD_TYPE == SSD_COM_CATHODE
		MDIO_vSetPortVal(SSD_PORT, ~sevenSegment[A_u8Num]);

#elif SSD_TYPE == SSD_COM_ANODE
		MDIO_vSetPortVal(SSD_PORT, sevenSegment[A_u8Num]);

#endif
	}
}
