/*
 * STEPPER_int.h
 *
 *  Created on: Jul 22, 2025
 *      Author: acer
 */

#ifndef HAL_STEPPER_STEPPER_INT_H_
#define HAL_STEPPER_STEPPER_INT_H_

#define STEPPER_PORT DIO_PORTC
#define STEPPER_BLUE_T DIO_PIN4
#define STEPPER_PINK_T DIO_PIN5
#define STEPPER_YELLOW_T DIO_PIN6
#define STEPPER_ORANGE_T DIO_PIN7

void STEPPER_vInit(void);
void STEPPER_vRotateCW(f64 A_f64Speed); //TODO: input angle
void STEPPER_vRotateCCW(f64 A_f64Speed); //TODO: input angle
void STEPPER_vStop();


#endif /* HAL_STEPPER_STEPPER_INT_H_ */
