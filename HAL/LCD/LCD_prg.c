#include <util/delay.h>
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/DIO_int.h"
#include "LCD_int.h"

void HLCD_vInit(){
	//configure directions
	MDIO_vSetPortDir(LCD_DATA_PORT, 0xff);
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_RS, DIO_HIGH);
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_RW, DIO_HIGH);
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_E, DIO_HIGH);

	//power on
	_delay_ms(50);

	//function set
	HLCD_vSendCmd(0b00111000);
	_delay_ms(1);

	//display on/off
	HLCD_vSendCmd(0b00001110);
	_delay_ms(1);

	//display clear
	HLCD_vSendCmd(0b00000001);
	_delay_ms(2);

	//entry mode
	HLCD_vSendCmd(0b00000110);

}

void HLCD_vSendCmd(u8 A_u8Cmd){
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RS, DIO_LOW);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RW, DIO_LOW);

	MDIO_vSetPortVal(LCD_DATA_PORT, A_u8Cmd);

	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_HIGH);
	_delay_ms(1);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_LOW);
	_delay_ms(1);
}

void HLCD_vSendChar(u8 A_u8Char){
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RS, DIO_HIGH);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RW, DIO_LOW);

	MDIO_vSetPortVal(LCD_DATA_PORT, A_u8Char);

	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_HIGH);
	_delay_ms(1);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_LOW);
	_delay_ms(1);

}

void HLCD_vSendStr(u8 *A_str){
    while(*A_str != '\0'){
        HLCD_vSendChar(*A_str);
        A_str++;
    }
}
void HLCD_vSendNumber(u32 A_u32Num){
	u8 L_u8SaveDigit[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	s8 L_s8Size = 0;
	while(A_u32Num != 0){
		L_u8SaveDigit[L_s8Size] = A_u32Num % 10 + '0';
		A_u32Num /= 10;
		L_s8Size++;
	}
	for(s8 L_s8index = L_s8Size - 1; L_s8index>=0; L_s8index--){
		HLCD_vSendChar(L_u8SaveDigit[L_s8index]);
	}
}

void HLCD_vSetCursorPosition(u8 A_u8Row, u8 A_u8Col){
	switch(A_u8Row){
	case 0: HLCD_vSendCmd(DDRAM_BASE_ADDR_R0 + A_u8Col); break;
	case 1: HLCD_vSendCmd(DDRAM_BASE_ADDR_R1 + A_u8Col); break;
	}
}
void HLCD_vStoreCustomChar(u8 A_u8Location, u8* A_u8Pattern){
	if(A_u8Location < 8){
		HLCD_vSendCmd(CGRAM_BASE_ADDR + 8 * A_u8Location);
		for(u8 i = 0; i < 8; i++){
			HLCD_vSendChar(A_u8Pattern[i]);
		}
	}
}
void HLCD_vDisplayCustomChar(u8 A_u8Location){
	HLCD_vSendChar(A_u8Location);
}

void HLCD_vClearScreen(void) {
    HLCD_vSendCmd(0x01);
}
