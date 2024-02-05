

#include "STD_TYPES.h"
#include "bit_math.h"
#include "DIO_interface.h"
#include "PORT_interface.h"
#include "GIE_interface.h"
#include "EXTI_interface.h"
#include "CLCD_interface.h"
#include "KPD_interface.h"
#include "DC_MOTOR_interface.h"
#include "SWITCH_interface.h"
#include "LED_interface.h"
#include <util/delay.h>
#include "Test2.h"

void int2_func(void);

uint8 counter=0u;

motor_config Motor={
		.port=DIO_u8PORTC,
		.pin1=DIO_u8PIN3,
		.pin2=DIO_u8PIN4
};

led_config_t GreenLed={
		.port=DIO_u8PORTC,
		.pin=DIO_u8PIN1,
		.ActiveType=ACT_HIGH
};
led_config_t RedLed={
		.port=DIO_u8PORTC,
		.pin=DIO_u8PIN2,
		.ActiveType=ACT_HIGH

};
void main(void)
{


	PORT_voidInit();
	CLCD_voidInit();

	EXTI_voidInit();
	EXTI_u8SetCallBack(INT2, &int2_func);
	GIE_voidEnableGlobal();


	uint8 Hamoksha1[] = {
			0b01110,
			0b01110,
			0b01110,
			0b00100,
			0b01100,
			0b00110,
			0b01100,
			0b10010
	};
	uint8 Hamoksha2[] = {
			0b01110,
			0b01110,
			0b01110,
			0b00100,
			0b00110,
			0b01100,
			0b00110,
			0b01001
	};


	uint8 i;
	uint8 value;
	uint16 pass=0;
	uint16 ID=0;
	uint8 lcd_col=2;
	uint8 lcd_colID=12;
	uint8 PassCounter=0;
	uint8 flag1=0,flag2=0,flag = 0;

	while(1)
	{

		flag =0;
		flag1=0;
		flag2=0;

		if(counter%2==1)
		{
			EXTI_u8DisableIntChannel(INT2);
			CLCD_voidSendStringPos(1,5,"Welcome" );
			for(i=1;i<=16;i++)
			{
				CLCD_u8SendSpecialCharacter(1,Hamoksha1,2,i);
				_delay_ms(100);
				CLCD_voidSendDataPos(2,i++,' ');
				CLCD_u8SendSpecialCharacter(2,Hamoksha2,2,i);
				_delay_ms(100);
				CLCD_voidSendDataPos(2,i,' ');


			}

			CLCD_voidSendCommand(CLCD_CLEAR);
			CLCD_voidSendStringPos(1,1,"Enter ID : " );
			while(1)
			{
				value=KPD_8uGetPressedKey();
				if(value>='0' && value<='9')
				{
					CLCD_voidSendDataPos(1,lcd_colID++,value);
					value-='0';
					ID=ID*10+value;

				}
				if(value=='=')
				{
					CLCD_voidSendCommand(CLCD_CLEAR);
					value=0;
					lcd_colID=12;
					break;
				}

			}
			CLCD_voidSendStringPos(1,1,"Enter Password " );
			while(1)
			{
				value=KPD_8uGetPressedKey();
				if(value>='0' && value<='9')
				{
					CLCD_voidSendDataPos(2,lcd_col,value);
					_delay_ms(25);
					CLCD_voidSendDataPos(2,lcd_col++,'*');

					value-='0';
					pass=pass*10+value;
					PassCounter++;
				}

				if(PassCounter==4)
				{

					if(pass==1953)
					{
						CLCD_voidSendCommand(CLCD_CLEAR);
						value=0;
						PassCounter=0;
						lcd_col=2;

						flag =1;
						flag1=1;
						flag2=0;

						EXTI_u8EnableIntChannel(INT2);
						CLCD_voidSendStringPos(1,5,"System ON" );
						LED_u8SetON(&GreenLed);

						while(counter%2==1)
						{
							Motor_voidRotateCCW(&Motor);
							_delay_ms(800);


							if(!(counter%2==1))
								break;

							Motor_voidRotateCW(&Motor);
							_delay_ms(800);
						}
						if(!(counter%2==1))
						{
							CLCD_voidSendCommand(CLCD_CLEAR);
							CLCD_voidSendStringPos(1,3,"System Close" );
							LED_u8SetON(&RedLed);

							for(i=16;i>0;i--)
							{
								CLCD_u8SendSpecialCharacter(1,Hamoksha1,2,i);
								_delay_ms(100);
								CLCD_voidSendDataPos(2,i--,' ');
								CLCD_u8SendSpecialCharacter(2,Hamoksha2,2,i);
								_delay_ms(100);
								CLCD_voidSendDataPos(2,i,' ');

							}
							EXTI_u8EnableIntChannel(INT2);
							LED_u8SetOFF(&RedLed);
							CLCD_voidSendCommand(CLCD_CLEAR);
							break;
							/*
							CLCD_voidSendCommand(CLCD_CLEAR);
							Motor_voidStop(&Motor);
							LED_u8SetOFF(&GreenLed);
							break;
							 */
						}


					}
					else
					{
						PassCounter=0;
						flag2++;
						pass=0;
						CLCD_voidSendCommand(CLCD_CLEAR);
						CLCD_voidSendStringPos(1,2,"INCORRECT PASS");
						_delay_ms(300);
						CLCD_voidSendCommand(CLCD_CLEAR);

						switch(flag2)
						{
						case 1:
							CLCD_voidSendStringPos(1,5,"REMINDER");
							CLCD_voidSendStringPos(2,6,"2 TRIES");
							_delay_ms(500);
							break;
						case 2:
							CLCD_voidSendStringPos(1,5,"REMINDER ");
							CLCD_voidSendStringPos(2,6,"1 TRY");
							_delay_ms(500);
							break;

						}

						lcd_col=2;
						CLCD_voidSendCommand(CLCD_CLEAR);
						if(flag2<3)
						{
							CLCD_voidSendStringPos(1,1,"Enter Password  ");
						}
						else
						{
							CLCD_voidSendCommand(CLCD_CLEAR);
							CLCD_voidSendStringPos(1,3,"System Close" );
							LED_u8SetON(&RedLed);

							for(i=16;i>0;i--)
							{
								CLCD_u8SendSpecialCharacter(1,Hamoksha1,2,i);
								_delay_ms(100);
								CLCD_voidSendDataPos(2,i--,' ');
								CLCD_u8SendSpecialCharacter(2,Hamoksha2,2,i);
								_delay_ms(100);
								CLCD_voidSendDataPos(2,i,' ');

							}
							counter++;
							EXTI_u8EnableIntChannel(INT2);
							LED_u8SetOFF(&RedLed);
							CLCD_voidSendCommand(CLCD_CLEAR);
							break;

						}
					}
				}

			}
		}


	}


}


void int2_func(void)
{
	CLCD_voidSendCommand(CLCD_CLEAR);
	counter++;
	if(counter%2==0)
	{
		Motor_voidStop(&Motor);
		LED_u8SetOFF(&GreenLed);
	}
}
