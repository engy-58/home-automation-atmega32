/*
 * TIMERS_prv.h
 *
 */

#ifndef MCAL_TIMERS_TIMERS_PRV_H_
#define MCAL_TIMERS_TIMERS_PRV_H_

/* --------- Timer 0 --------- */
#define TCCR0	*((volatile u8*)(0x53))
#define TCNT0	*((volatile u8*)(0x52))
#define OCR0	*((volatile u8*)(0x5C))
#define TIMSK	*((volatile u8*)(0x59))
#define TIFR	*((volatile u8*)(0x58))

/* --------- Timer 1 --------- */
#define TCCR1A	*((volatile u8*) (0x4F))
#define TCCR1B	*((volatile u8*) (0x4E))
#define TCNT1	*((volatile u16*)(0x4C))
#define OCR1A	*((volatile u16*)(0x4A))
#define OCR1B	*((volatile u16*)(0x48))
#define ICR1	*((volatile u16*)(0x46))


/* ---------------------- Timer Configuration ----------------------  */
#define ENABLE	1
#define DISABLE	0


/* ---------- CLK PRESCALLER ---------- */
#define NO_CLK				0x00
#define CLK_NO_PRESCALLING	0x01
#define CLK_8				0x02
#define CLK_64				0x03
#define CLK_256				0x04
#define CLK_1024			0x05
#define EXTERNAL_T0_FALLING	0x06
#define EXTERNAL_T0_RISING	0x07

/* WAVEFORM MODES */
#define NORMAL_OVERFLOW		0
#define PWM_PHASE_CORRECT	1
#define CTC					2
#define FAST_PWM			3


/* --------------------------------- Registers ---------------------------------*/
/* TCCR0 */
#define FOC0	7
#define WGM01	6
#define COM01	5
#define COM00	4
#define WGM00	3
#define CS02	2
#define CS01	1
#define CS00	0


#endif /* MCAL_TIMERS_TIMERS_PRV_H_ */
