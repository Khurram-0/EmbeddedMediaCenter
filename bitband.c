#include "LPC17xx.h"
#include <stdio.h>
#include <time.h>
#include "Board_LED.h"
#include "bitband.h"
#include "KBD.h"
#include "GLCD.h"

//------- ITM Stimulus Port definitions for printf ------------------- //
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

#define __FI        1        /* Font index 16x24         */
#define __USE_LCD   0

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
//------------------------------------------------------------------- //

// Bit Band Macros used to calculate the alias address at run time
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))

volatile unsigned long * bit;
// Calculations of below addresses are in report
#define BIT_P1_28 (*((volatile unsigned long *)0x233806F0)) // LED P1.28 Bitband word address
#define BIT_P2_5 (*((volatile unsigned long *)0x23380A94)) // LED P2.5 Bitband word address


// Delay function using SysTick, takes time in milliseconds
volatile uint32_t msTicks;       
void SysTick_Handler(void) {
	msTicks++;
}
void Delay (uint32_t dlyTicks) {
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks) { __NOP(); }
}	

// Setup character arrays for LCD display  
char led[15];
char text[15];
char eqn[15];
char barrel[15];

int main(void){
	int delay_val = 200; 				// Delay amount: 200 ms
	int Reg1 = 0x233806F0;	// Reg1, for BIT_P1_28
	int Reg2 = 0x23380A94;	// Reg2, for BIT_P2_5
	int Reg3 = 0;						// Reg3, for barrel shifter result
	
	int32_t  Joystick_val;			// Value of Joystick
	/* Initialize graphical LCD (if enabled) */
	#ifdef __USE_LCD
	GLCD_Init();                               
	GLCD_Clear(Black);		/* Clear graphical LCD display   */
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(4, 15, __FI, "Main");
	GLCD_DisplayString(5, 15, __FI, "Menu");
	GLCD_SetTextColor(Cyan);
	GLCD_DisplayString(2, 5, __FI, " GALLERY ");
	GLCD_DisplayString(5, 0, __FI, " MUSIC ");
	GLCD_DisplayString(8, 6, __FI, " GAME ");
	#endif
	
	SysTick_Config(SystemCoreClock / 1000);		// Delay timer Initialization
	LED_Initialize();													// LEDs Initialization
	KBD_Init();																// Joystick Initialization
	
	int current_selection=0; //0=GALLERY, 1=MUSIC Player, 2=GAME
	
// Run loop forever
while(1){
	Joystick_val = get_button(); // Get & store current value of joystick
	
	if (Joystick_val == KBD_UP)	{
		GLCD_Clear(Black);
		GLCD_SetTextColor(White);
		GLCD_DisplayString(4, 15, __FI, "Main");
		GLCD_DisplayString(5, 15, __FI, "Menu");
		GLCD_SetTextColor(Cyan);
		GLCD_DisplayString(5, 0, __FI, " MUSIC ");
		GLCD_DisplayString(8, 6, __FI, " GAME ");
		
		GLCD_DisplayString(2, 5, __FI, ">GALLERY<");
		
		current_selection=0;
	}
	else if (Joystick_val == KBD_LEFT) {
		GLCD_Clear(Black);		
		GLCD_SetTextColor(White);
		GLCD_DisplayString(4, 15, __FI, "Main");
		GLCD_DisplayString(5, 15, __FI, "Menu");
		GLCD_SetTextColor(Cyan);		
		GLCD_DisplayString(2, 5, __FI, " GALLERY ");
		GLCD_DisplayString(8, 6, __FI, " GAME ");

		GLCD_DisplayString(5, 0, __FI, ">MUSIC<");
		
		current_selection=1;
				
	}
	else if (Joystick_val == KBD_DOWN) {
		GLCD_Clear(Black);		
		GLCD_SetTextColor(White);
		GLCD_DisplayString(4, 15, __FI, "Main");
		GLCD_DisplayString(5, 15, __FI, "Menu");
		GLCD_SetTextColor(Cyan);
		GLCD_DisplayString(2, 5, __FI, " GALLERY ");
		GLCD_DisplayString(5, 0, __FI, " MUSIC ");
		
		GLCD_DisplayString(8, 6, __FI, ">GAME<");
		current_selection=2;
		
	}
	// Go into SUB MENU
	else if (Joystick_val == KBD_RIGHT) {	
		if (current_selection == 0){ //0=gallery
				GLCD_Clear(Black);		/* Clear graphical LCD display   */
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				Delay(100);
				GLCD_DisplayString(4, 5, __FI, " GALLERY ");
		}
		else if (current_selection == 1){ //1=Music
				GLCD_Clear(Black);		/* Clear graphical LCD display   */
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				Delay(100);
				GLCD_DisplayString(4, 5, __FI, " MUSIC PLAYER ");

		}
		else if (current_selection == 2){ //2=game
				GLCD_Clear(Black);		/* Clear graphical LCD display   */
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				Delay(100);
				GLCD_DisplayString(4, 6, __FI, " GAME ");
		}
	
	}

	
}	
	//return 0;
}
