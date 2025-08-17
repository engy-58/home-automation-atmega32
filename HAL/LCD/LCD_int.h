
#ifndef HAL_LCD_LCD_INT_H_
#define HAL_LCD_LCD_INT_H_

#define LCD_CTRL_PORT 	 		DIO_PORTA
#define LCD_DATA_PORT 	 		DIO_PORTB
#define LCD_RS			 		DIO_PIN0
#define LCD_RW			 		DIO_PIN1
#define LCD_E			 		DIO_PIN2
#define DDRAM_BASE_ADDR_R0		0x80
#define DDRAM_BASE_ADDR_R1		0xC0
#define CGRAM_BASE_ADDR 		0x40


void HLCD_vInit();
void HLCD_vSendCmd(u8 A_u8Cmd);
void HLCD_vSendChar(u8 A_u8Char);
void HLCD_vSendStr(u8 *A_str);
void HLCD_vSendNumber(u32 A_u32Num);
void HLCD_vSetCursorPosition(u8 A_u8Row, u8 A_u8Col);
void HLCD_vStoreCustomChar(u8 A_u8Location, u8* A_u8Pattern);
void HLCD_vDisplayCustomChar(u8 A_u8Location);
void HLCD_vClearScreen(void);

#endif
