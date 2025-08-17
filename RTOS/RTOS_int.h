/*
 * RTOS_int.h
 */

#ifndef RTOS_RTOS_INT_H_
#define RTOS_RTOS_INT_H_


void RTOS_vStartOS(void);
u8 RTOS_u8CreateTask(void (*Fptr)(void), u16 A_u16Periodicity, u8 A_u8Priority, u8 A_u8FirstDelay);
u8 RTOS_u8DeleteTask(u8 A_u8Priority);
u8 RTOS_u8SuspendTask(u8 A_u8Priority);
u8 RTOS_u8ResumeTask(u8 A_u8Priority);



/* ---- Error States ---- */
/*
 * OUT_OF_RANGE		    1
 * PRIORITY_COLLISION   2
 * NO_TASK_EXISTS		3
 *
 */

#endif /* RTOS_RTOS_INT_H_ */
