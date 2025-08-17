/*
 * GE_prg.c
 *
 *  Created on: Jul 24, 2025
 *      Author: acer
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "GIE_prv.h"


void MGIE_vEnableGlobalInterrupt(void){
    SET_BIT(SREG, 7);
}

void MGIE_vDisableGlobalInterrupt(void){
    CLR_BIT(SREG, 7);
}
