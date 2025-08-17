/*
 * RTOS_prg.c
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

/**************************************************/

/**************************************************/
#include "../MCAL/TIMERS/TIMERS_int.h"

#include "RTOS_int.h"
#include "RTOS_prv.h"
#include "RTOS_cfg.h"


/**************************************************/
/* Global Variables */
/**************************************************/
static Task_t G_xSystemTasks[MAX_NUM_OF_TASKS] = {{0}};
static u16 TaskTiming[MAX_NUM_OF_TASKS] = {0};


/* Scheduler function: runs tasks based on timing and state */
static void RTOS_vScheduler(void)
{
	for(u8 A_u8Counter = 0; A_u8Counter < MAX_NUM_OF_TASKS ; A_u8Counter++)
	{

		/* Check whether there is a registered task */
		if(G_xSystemTasks[A_u8Counter].TaskFunction != 0)
		{
			/* Is this a running task? */
			if(G_xSystemTasks[A_u8Counter].TaskRunningState == Running)
			{
				/* in case of tasktimer reached zero or overflow happened */
				if(TaskTiming[A_u8Counter] == 0 || (TaskTiming[A_u8Counter] > G_xSystemTasks[A_u8Counter].Periodicity))
				{
					/* Call function -> Run this task */
					G_xSystemTasks[A_u8Counter].TaskFunction();

					/* Reload task timing */
					TaskTiming[A_u8Counter] = G_xSystemTasks[A_u8Counter].Periodicity ;
				}else
				{
					TaskTiming[A_u8Counter]--;
				}

			}
		}

	}
}

				/* ************************************************** */

/* Start the RTOS and configure timer interrupts */
void RTOS_vStartOS(void)
{
	MTIMERS_vInit();
	MTIMERS_vSetInterval_CTC(RTOS_vScheduler, COUNT, OCR_VAL );	// tick time 10ms     ocr? 124, count? 80
	MTIMERS_vStartTimer(TIM_0); // ps? 8

}

/* Create a new task with given priority and timing */
u8 RTOS_u8CreateTask(void (*Fptr)(void), u16 A_u16Periodicity, u8 A_u8Priority, u8 A_u8FirstDelay)
{
	u8 L_u8ErrorState = 0;

	/* Range Validation */
	if((A_u8Priority >=0) && (A_u8Priority < MAX_NUM_OF_TASKS))
	{
		if(G_xSystemTasks[A_u8Priority].TaskFunction == 0)
		{
			G_xSystemTasks[A_u8Priority].TaskFunction = Fptr;
			G_xSystemTasks[A_u8Priority].Periodicity = A_u16Periodicity;
			G_xSystemTasks[A_u8Priority].TaskRunningState = Running;

			TaskTiming[A_u8Priority] = A_u8FirstDelay;

		}else
		{
			/* Reserved task with the same priority */
			L_u8ErrorState = PRIORITY_COLLISION;
		}
	}
	else
	{
		L_u8ErrorState = OUT_OF_RANGE;
	}



	return L_u8ErrorState;
}


/* Delete a task by priority */
u8 RTOS_u8DeleteTask(u8 A_u8Priority)
{
	u8 L_u8ErrorState = 0;

	/* Check whether there is a registered task */
	if(G_xSystemTasks[A_u8Priority].TaskFunction != 0)
	{
		G_xSystemTasks[A_u8Priority] = (Task_t){0};
	}else
	{
		/* Task doesn't exist */
		L_u8ErrorState = NO_TASK_EXISTS;
	}

	return L_u8ErrorState;
}
/* Suspend a task by priority */
u8 RTOS_u8SuspendTask(u8 A_u8Priority)
{
	u8 L_u8ErrorState = 0;

	/* Check whether there is a registered task */
	if(G_xSystemTasks[A_u8Priority].TaskFunction != 0)
	{
		G_xSystemTasks[A_u8Priority].TaskRunningState = Suspended;
	}else
	{
		/* Task doesn't exist */
		L_u8ErrorState = NO_TASK_EXISTS;
	}

	return L_u8ErrorState;
}

/* Resume a suspended task by priority */
u8 RTOS_u8ResumeTask(u8 A_u8Priority)
{
	u8 L_u8ErrorState = 0;

	/* Check whether there is a registered task */
	if(G_xSystemTasks[A_u8Priority].TaskFunction != 0)
	{
		G_xSystemTasks[A_u8Priority].TaskRunningState = Running;
	}else
	{
		/* Task doesn't exist */
		L_u8ErrorState = NO_TASK_EXISTS;
	}

	return L_u8ErrorState;
}
