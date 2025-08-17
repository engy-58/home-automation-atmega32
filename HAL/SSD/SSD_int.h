/*
 * SSD_int.h
 *
 *  Created on: Jul 20, 2025
 *      Author: acer
 */
#ifndef HAL_SSD_SSD_INT_H_
#define HAL_SSD_SSD_INT_H_

#define  SSD_PORT 			DIO_PORTA
#define  SSD_COM_CATHODE	0
#define  SSD_COM_ANODE 		1
#define  SSD_TYPE 			SSD_COM_CATHODE

void HSSD_vInit();
void HSSD_vDisplayNumaber(u8 A_u8Num);

#endif /* HAL_SSD_SSD_INT_H_ */
