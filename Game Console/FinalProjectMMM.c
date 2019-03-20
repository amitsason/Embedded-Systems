// 
//
//Game Console by Amit Sason 203096003 and Maty Ripa 332740026
//It uses GPIO, PWM, TIMER, INTERUPT, KEYPAD and LCD

//For Simon Say Game:
//KEYPAD: 1 == RED LED
//				3 == GREEN LED
//				7 == BLUE LED
//				9 == WHITE LED
//EXTERNAL BUZZER IS CONNECTED TO GPA15 and GND

//For Catch Blue LED Game:
//KEYPAD: 5 == Catch a LED
//				9 == Continue

//Make sure you have all the additional drivers and header files in your folder:
//Source Files:
//1-LCD_Driver.c  2-Ascii_Table.C  3-ScanKey.c 4-Driver_PWM.c  
//Library Files:
//1-DrvSYS.c  2-DrvGPIO.c  3-DrvADC.c
//
#define  tone_r C5			// red LED tone (Hex)
#define	 tone_g D5			// green LED tone (Hex)
#define  tone_b E5			// blue LED tone (Hex)
#define  tone_w F5			// white LED tone (Hex)
#define  DO   C5
#define  RE   D5
#define  MI   E5
#define  FA   F5
#define  SOL  G5
#define  LA   A5
#define  SI   B5
#define  DOd  C6
#define  ONESHOT  0     // counting and interrupt when reach TCMPR number, then stop
#define  PERIODIC 1     // counting and interrupt when reach TCMPR number, then counting from 0 again
#define  TOGGLE   2     // keep counting and interrupt when reach TCMPR number, tout toggled (between 0 and 1)
#define  CONTINUOUS 3   // keep counting and interrupt when reach TCMPR number
#define  SPEED1  100000 // game speed
#define  SPEED2	 50000	// game speed
#define  SPEED3  25000  // game speed
#define  duty_cycle 50  // PWM duty cycle 50%
#define  PWM_PIN 3			// choose the PWM pin: PWM0 / GPA12 : pin 65
																						//•PWM1 / GPA13 : pin 64
																						//•PWM2 / GPA14 : pin 63
																						//•PWM3 / GPA15 : pin 62
																						//•PWM4 / GPB11 : pin 48
																						//•PWM5 / GPE5  : pin 47
																						//•PWM6 / GPE0  : pin 55
																						//•PWM7 / GPE1  : pin 54




#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "DrvGPIO.h"
#include "scankey.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvUART.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvADC.h"
#include "Driver_PWM.h"
#include "note_freq.h"

uint32_t ss = 0;           // miliseconds variable for Timer and Interupt to plant as a seed to srand()

void Init_LED();           // Set all LED's as output and turn off for Simon Says
void Init_LED2();					 // Set all LED's as output and turn off for CatchBlueLED
void show_sequence(int level, int sequence[], int speed);  //display the sequence each round
int get_sequence(int level,int sequence[], char TEXT0[], int *ip); // collect user input and compare it to sequence
void Toggle_LED(int input); // turn on input LED
void TurnOffLED(void);      // turn off all LED
void InitTIMER0(void);      // initialize timer to tick every 1ms
void TMR0_IRQHandler(void); // initialize Interupt that increment ss by 1 every 1 ms
void sad_noise(void);       // generate a sad noise
void happy_noise(void);			// generate a happy noise


int32_t main (void)
{
	char TEXT0[16]="Choose a Game:  ";
  char TEXT1[16]="Saimon Says   >1";
	char TEXT2[16]="Catch Blue LED>2";
	int flag4 = 0;
	int flag;
	int32_t a;
	
	InitPWM(PWM_PIN); 						        // initialize PWM3, output pin = GPA15

  UNLOCKREG();													//
  DrvSYS_Open(48000000);                // set MCU to run at 48MHz
	DrvSYS_Delay(5000);                   // wait till 12MHz crystal stable
	LOCKREG();
	
	happy_noise();                        // generate a happy sound
	happy_noise();												// generate a happy sound
	happy_noise();												// generate a happy sound

	Initial_panel(); 											// initialize LCD
	clr_all_panel();											// clear LCD
	OpenKeyPad();	       			          	// initialize 3x3 keypad
	
	print_lcd(0, TEXT0);         					// display "Choose a Game:  "    on LCD
	print_lcd(1, TEXT1);         				  // display "aimon Says   >1"         on LCD
	print_lcd(2, TEXT2);        				  // display "Catch Blue LED>2" on LCD
	
	while (!Scankey())										// wait untill you press keypad
	{}

	while(!flag4)  												// continue only if input is 1 or 2
		{
		flag = Scankey()-1;
		if(flag == 0 || flag == 1 )
		{
			flag4 = 1;
		}
		else
			flag4 = 0;
  	}
	switch (flag) 											  // determine the game speed
		{
			case 0:       
			a = SaimonSays();                 // Start Playing SaimonSays
		  break;
			
			case 1:       
			a = CatchBlueLED();								// Start Playing CatchBlueLED
		  break;
			
		}
	
	
	
}


int32_t CatchBlueLED (void)
{
	int8_t number;
	char TEXT0[16]="PRESS5 WHEN BLUE";
	char TEXT1[16]="YOU WIN";
	char TEXT2[16]="YOU LOSE";
	char TEXT3[16]="9 TO RESTART";
	char TEXT4[16]="score: ";
	int secuence [10];
	int i;
	int score = 0;
	int flag = 0;
	
	UNLOCKREG();
    DrvSYS_Open(48000000); // set MCU to run at 48MHz
	LOCKREG();

	Initial_panel(); 
	clr_all_panel();
	happy_noise();

	OpenKeyPad();	    // initialize 3x3 keypad
	
	Init_LED2();
	 
	while(1)
	{
		print_lcd(0,TEXT0); // print title
		for (i = 0; i<10; i++)
		{
			secuence[i] = (rand() % 3);
		}
		for (i=0; i<10; i++)
		{
			switch (secuence[i])
			{
				case 0:
									// set RGBled to Blue
							DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
							DrvGPIO_SetBit(E_GPA,13); 
							DrvGPIO_SetBit(E_GPA,14); 
//							DrvSYS_Delay(90000000); 
//							DrvSYS_Delay(90000000); 
							break;
				
				case 1: 
										// set RGBled to Green
							DrvGPIO_SetBit(E_GPA,12); 
							DrvGPIO_ClrBit(E_GPA,13); // GPA13 = Green, 0 : on, 1 : off
							DrvGPIO_SetBit(E_GPA,14); 
//							DrvSYS_Delay(90000000);
//							DrvSYS_Delay(90000000); 
							break;
				
				case 2:
										// set RGBled to Red
							DrvGPIO_SetBit(E_GPA,12); 
							DrvGPIO_SetBit(E_GPA,13); 
							DrvGPIO_ClrBit(E_GPA,14); // GPA14 = Red,   0 : on, 1 : off
//							DrvSYS_Delay(90000000);
//							DrvSYS_Delay(90000000); 
							break;
			}
			number = Scankey(); 	      // scan keypad to input
			//while (Scankey());
			
			if (number == 5)
				break;
			
			DrvSYS_Delay(90000000); 
			number = Scankey();
			if (number == 5)
				break;
			DrvSYS_Delay(90000000); 
			number = Scankey();
			if (number == 5)
				break;
			
		}
		if ((number == 5) && (secuence[i] == 0))
			{
				score++;
				clr_all_panel();
				sprintf(TEXT4+7,"%d",score); // print scankey input to string			  
				print_lcd(0, TEXT1);          // display string on LCD
				print_lcd(1, TEXT4);          // display string on LCD
				print_lcd(2, TEXT3);          // display string on LCD
				DrvGPIO_ClrBit(E_GPB,11); // GPB11 = 0 to turn on Buzzer
				DrvSYS_Delay(600000);	    // Delay 
				DrvGPIO_SetBit(E_GPB,11); // GPB11 = 1 to turn off Buzzer	
				//DrvSYS_Delay(100000);	    // Delay 		
				flag = 1;
			
			}
			
			else if ((number == 5) && (secuence[i] != 0))
			{
				clr_all_panel();	
				sprintf(TEXT4+7,"%d",score); // print scankey input to string			  
				print_lcd(1, TEXT4);          // display string on LCD
				print_lcd(0, TEXT2);          // display string on LCD
				print_lcd(2, TEXT3);          // display string on LCD
				DrvGPIO_ClrBit(E_GPB,11); // GPB11 = 0 to turn on Buzzer
				DrvSYS_Delay(100000);	    // Delay 
				DrvGPIO_SetBit(E_GPB,11); // GPB11 = 1 to turn off Buzzer	
				DrvSYS_Delay(100000);	    // Delay 		
				DrvGPIO_ClrBit(E_GPB,11); // GPB11 = 0 to turn on Buzzer
				DrvSYS_Delay(100000);	    // Delay 
				DrvGPIO_SetBit(E_GPB,11); // GPB11 = 1 to turn off Buzzer	
				DrvSYS_Delay(100000);	    // Delay 		
				DrvGPIO_ClrBit(E_GPB,11); // GPB11 = 0 to turn on Buzzer
				DrvSYS_Delay(100000);	    // Delay 
				DrvGPIO_SetBit(E_GPB,11); // GPB11 = 1 to turn off Buzzer	
				//DrvSYS_Delay(100000);	    // Delay 	
				score = 0;
				flag = 2;
	
			}
			
			
			
		while (flag == 1)
		{
					// set RGBled to Blue
							DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
							DrvGPIO_SetBit(E_GPA,13); 
							DrvGPIO_SetBit(E_GPA,14);
							number = Scankey();
							if (number == 9)
							{
								flag = 0;
								clr_all_panel();
								
							}
		}
		
		while (flag == 2)
		{
					// set RGBled to Red
							DrvGPIO_SetBit(E_GPA,12); 
							DrvGPIO_SetBit(E_GPA,13); 
							DrvGPIO_ClrBit(E_GPA,14); // GPA14 = Red,   0 : on, 1 : off
							number = Scankey();
							if (number == 9)
							{
								flag = 0;
								clr_all_panel();
								
							}
		}
		
																	 
	}
}



int32_t SaimonSays (void)
{
	char TEXT0[16]="you lose";
	char TEXT1[16]="New Game->     1";
	char TEXT2[16]="Start Game      ";
	char TEXT3[16];
	char TEXT4[16]="Change speed-> 3";
	char TEXT5[16];
	char TEXT6[16]="choose speed:";
	char TEXT7[16]="1 = Low";
	char TEXT8[16]="2 = Medium";
	char TEXT9[16]="3 = High";
	char TEXT10[16]="Change speed-> 3";
	char TEXT11[16]="You Win!";
	char TEXT12[16]="Level 100!";


	int level = 1;
	int new_level, my_rand;
	int *ip = &level;                     //pointer to level (will be used by get_sequence() function)
	int i,time3, flag = 0, flag4 = 0;     //
	int sequence[100];                    //maximal sequence is with length of 100
	int j, n, flag1 = 0;									//
  int speed;														//
	int max_score =0;
	uint8_t  number3;											//
	uint16_t tone;												//

  DrvGPIO_Open(E_GPB, 11, E_IO_OUTPUT); // initial GPIO pin GPB11 for controlling Buzzer
	UNLOCKREG();													//
  DrvSYS_Open(48000000);                // set MCU to run at 48MHz
	SYSCLK->PWRCON.XTL12M_EN = 1;					//
	SYSCLK->CLKSEL0.HCLK_S = 0;						//
	DrvSYS_Delay(5000);                   // wait till 12MHz crystal stable
	LOCKREG();
	Init_LED();
	Initial_panel(); 
	clr_all_panel();
	OpenKeyPad();	       			  	// initialize 3x3 keypad
	DrvSYS_Delay(48000000);	      //
	InitTIMER0();         				// initialize timer0
	InitPWM(PWM_PIN); 						// initialize PWM3, output pin = GPA15
	clr_all_panel();	

	happy_noise();

	print_lcd(0, TEXT6);          // display "choose speed:"    on LCD
	print_lcd(1, TEXT7);          // display "1 = Easy"         on LCD
	print_lcd(2, TEXT8);          // display "2 = Intermidiate" on LCD
	print_lcd(3, TEXT9);          // display "3 = Hard"         on LCD
	while (!Scankey())						// wait untill you press keypad
	{}

	while(!flag4)  								// continue only if input is 1 2 or 3
		{
		flag = Scankey()-1;
		if(flag == 0 || flag == 1 || flag == 2 )
		{
		flag4 = 1;
		}
		else
		flag4 = 0;
  	}
	
	switch (flag) 							  // determine the game speed
		{
			case 0:       
			speed = SPEED1;
		  break;
			
			case 1:       
			speed = SPEED2;
		  break;
			
			case 2:       
			speed = SPEED3;
		  break;
		}
		
	clr_all_panel();							//
	flag4 = 0;	
	srand(ss);								    //seeds the clock value ss to random seed
	TIMER0->TCSR.CEN = 0;	        //DISABLE Timer0
	TIMER0->TCSR.IE = 0;          //DISABLE Interupt 

	for (i=0; i<=100; i++)        // generate the game sequence
		{
				sequence[i] = (rand() % 4);
		}
				
	print_lcd(1, TEXT2);          // display "Start Game" on LCD
	for(i=0;i<100000;i++)         // 
	{}													  //
	DrvSYS_Delay(48000000);		    // 
	clr_all_panel();
		
	while(1)                        				 // main loop of the program
			{	
			
				if(level == 100)									//check if you won and reached level 100
			{
		  clr_all_panel();
			print_lcd(1, TEXT11);              // display "You Win!" on LCD
			print_lcd(2, TEXT12);              // display "Level 100!" on LCD
		 	happy_noise();
			happy_noise();
			happy_noise();
	 		happy_noise();
			happy_noise();
			happy_noise();
			happy_noise();
	 		happy_noise();
			break;															// break out of the game loop
			}
			
			DrvGPIO_ClrBit(E_GPB,11);  				   // GPB11 = 0 to turn on Buzzer
			DrvSYS_Delay(100000);	      				 // Delay 
			DrvGPIO_SetBit(E_GPB,11);   				 // GPB11 = 1 to turn off Buzzer	
			DrvSYS_Delay(10000);	       				 // Delay 	
			
			new_level = level;
			show_sequence(level, sequence, speed);
			get_sequence(level, sequence, TEXT0, ip);
			
			if(level-1 > max_score)               // update the best score of player
					{
						max_score = level-1;
					}
			if (new_level == level)     				  // check if you lose
				{
				  clr_all_panel();
			  	print_lcd(1, TEXT0);              // display "you lose" on LCD
					sad_noise(); 											// make a sad noise
			  	DrvSYS_Delay(1000000);
		
					 print_lcd(1, TEXT1);             // display "New Game ->  1" on LCD
					 print_lcd(2, TEXT10);            // display "Change Speed -> 3" on LCD
					 flag4 = 0;
					 while(!flag4)  						  		// continue only if input is 1 or 3
					{
						flag = Scankey();
						if(flag == 1 || flag == 3 )     // if input is valid continue
							{
							flag4 = 1;
							}
						else
							flag4 = 0;
					}
						
						flag4 = 0;
						clr_all_panel();
						
						if(flag == 3)
						{
								clr_all_panel();	            //
								print_lcd(0, TEXT6);          // display "choose speed:"    on LCD
								print_lcd(1, TEXT7);          // display "1 = Easy"         on LCD
								print_lcd(2, TEXT8);          // display "2 = Intermidiate" on LCD
								print_lcd(3, TEXT9);          // display "3 = Hard"         on LCD
						  	while (Scankey())             // do nothing if keypad is still pressed
								{}

									
								while(!flag4)  								// continue only if input is 1 2 or 3
									{
									flag = Scankey()-1;
									if(flag == 0 || flag == 1 || flag == 2 )
									{
									flag4 = 1;
									}
									else
									flag4 = 0;
									}
									clr_all_panel();	           //

								switch (flag) 							   // determine the game speed
									{
										case 0:       
										speed = SPEED1;
										break;
										
										case 1:       
										speed = SPEED2;
										break;
										
										case 2:       
										speed = SPEED3;
										break;
									}
						}
						
					level = 1;                           // reset level 
					ss += 199;												   // change the random seed by 199
					srand(ss);
					for (i=0; i<=100; i++)               // generate new game sequence
						{
			        	sequence[i] = (rand() % 4);
						}
						
								print_lcd(1, TEXT2);           // display "Start Game" on LCD
				  			happy_noise();                 // make a happy sound
								for(i=0;i<100000;i++)          // delay
								{}													   // delay
								DrvSYS_Delay(48000000);		     // 
								clr_all_panel();
						}
						flag4 = 0;	

				 
			DrvSYS_Delay(48000000);		
			sprintf(TEXT3, "Score:      %d",level-1);     // display the Scroe on LCD
			print_lcd(0, TEXT3);         				          // display "score" on LCD	
			sprintf(TEXT5, "Best Score  %d",max_score);   // display the Scroe on LCD
			print_lcd(3, TEXT5);         				          // display "score" on LCD	

			}
		}


void sad_noise(void)       // makes a sad noise
{
				  PWM_Freq(PWM_PIN,tone_w,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
					PWM_Freq(PWM_PIN,tone_b,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
					PWM_Freq(PWM_PIN,tone_g,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
					PWM_Freq(PWM_PIN,tone_r,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
}
		
void happy_noise(void)     // make a happy noise
{
				  PWM_Freq(PWM_PIN,DO,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,RE,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,MI,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,FA,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,SOL,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,LA,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,SI,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
	
					PWM_Freq(PWM_PIN,DOd,duty_cycle); // set PWM output
					DrvSYS_Delay(SPEED3);		    // 
		      PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
}
void InitTIMER0(void)		   // Timer0 initialize to tick every 1ms
{
	/* Step 1. Enable and Select Timer clock source */          
	SYSCLK->CLKSEL1.TMR0_S = 0;	//Select 12Mhz for Timer0 clock source 
	SYSCLK->APBCLK.TMR0_EN =1;	//Enable Timer0 clock source

	/* Step 2. Select Operation mode */	
	TIMER0->TCSR.MODE = PERIODIC;		//Select once mode for operation mode

	/* Step 3. Select Time out period = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TIMER0->TCSR.PRESCALE = 11;	// Set Prescale [0~255]
	TIMER0->TCMPR = 1000;		// Set TCMPR [0~16777215]
	//Timeout period = (1 / 12MHz) * ( 11 + 1 ) * 1,000 = 1 ms

	/* Step 4. Enable interrupt */
	TIMER0->TCSR.IE = 1;
	TIMER0->TISR.TIF = 1;		//Write 1 to clear for safty		
	NVIC_EnableIRQ(TMR0_IRQn);	//Enable Timer0 Interrupt

	/* Step 5. Enable Timer module */
	TIMER0->TCSR.CRST = 1;	//Reset up counter
	TIMER0->TCSR.CEN = 1;		//Enable Timer0 ----------------------------------------------

//	TIMER0->TCSR.TDR_EN=1;		// Enable TDR function
}


void TMR0_IRQHandler(void) // Timer0 interrupt subroutine 
{

	static uint32_t Timing_Counter_1=0;

	Timing_Counter_1++;

	if(Timing_Counter_1==1)	//1 miliSecond period
	{
		Timing_Counter_1 = 0;
		ss++;                  //Increment ss by 1 every 1ms
	}

	TIMER0->TISR.TIF =1; 	   
}

 
 
void TurnOffLED(void)      // turn off all LED
{
		DrvGPIO_SetBit(E_GPA,12);   // set E_GPA12 to 1 (off)
		DrvGPIO_SetBit(E_GPA,13);   // set E_GPA13 to 1 (off)
    DrvGPIO_SetBit(E_GPA,14);   // set E_GPA14 to 1 (off)
	  return;
}
void Toggle_LED(int input) // turn on input LED
{
switch (input)
		{
			case 0:         //red led
			// set RGBled to Red
    DrvGPIO_SetBit(E_GPA,12); // set E_GPA12 to 1 (off)
    DrvGPIO_SetBit(E_GPA,13); // set E_GPA13 to 1 (off)
    DrvGPIO_ClrBit(E_GPA,14); // GPA14 = Red,   0 : on, 1 : off

		break;
			
			
			case 1:         //green led
			// set RGBled to Green
    DrvGPIO_SetBit(E_GPA,12); // set E_GPA12 to 1 (off)
    DrvGPIO_ClrBit(E_GPA,13); // GPA13 = Green, 0 : on, 1 : off
    DrvGPIO_SetBit(E_GPA,14); // set E_GPA14 to 1 (off)
			
		break;
			
			case 2:        // blue led
				// set RGBled to Blue
    DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
    DrvGPIO_SetBit(E_GPA,13); // set E_GPA13 to 1 (off)
    DrvGPIO_SetBit(E_GPA,14); // set E_GPA14 to 1 (off)

		break;			
			
			case 3:       // white led
			// set RGBled to white
    DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
    DrvGPIO_ClrBit(E_GPA,13); // GPA13 = Green, 0 : on, 1 : off
    DrvGPIO_ClrBit(E_GPA,14); // GPA14 = Red,   0 : on, 1 : off
 
		break;
			
		}

}

int get_sequence(int level,int sequence[], char TEXT0[], int *ip)  // collect user input and compare it to sequence
{
	int i,input,tone_t, flag3=0;
	for(i=0;i<level;i++)  // runs on every sequence up to current level
	{

		while(!flag3)      // passes only valid input 1 2 3 or 4
		{
		input = Scankey()-1;
		if(input == 0 || input == 2 || input == 6 || input == 8)
		{
		flag3 = 1;
		}
		else
		flag3 = 0;
  	}
		
		
			switch (input)   // determine tone of pressed button
		{
			case 0:       
			tone_t = tone_r;  // red tone
			input = 0;
		  break;
			
			case 2:       
			tone_t = tone_g;  // green tone
			input = 1;
		  break;
			
			case 6:       
			tone_t = tone_b;  // blue tone
			input = 2;
		  break;
			
			case 8:       
			tone_t = tone_w;  // white tone
			input = 3;
		  break;
		}
		
		PWM_Freq(PWM_PIN,tone_t,duty_cycle); // turn on PWM output with tone_t
		Toggle_LED(input);                   // Turn on desiered LED
		while (Scankey())                    // do nothing while button is pressed
		{}
		PWM_Freq(PWM_PIN,0,duty_cycle);     // turn off PWM output
		TurnOffLED();										  	// Turn off all LED's
		flag3 = 0;

		if(input != sequence[i])            // check if pressed button matches the sequence
			{
			 return 0;
			}
		else
		{
		 }

	}
	*ip +=1;                               // level pointer increment level
	return 1;

}
void show_sequence(int level, int sequence[],int speed)            //display the sequence each round
{
int i, seq;
	
	for(i=0;i<level;i++)
	{
	seq = sequence[i];
		switch (seq)
		{
			case 0:         //red led
			// set RGBled to Red
		PWM_Freq(PWM_PIN,tone_r,duty_cycle); // set PWM output
    DrvGPIO_SetBit(E_GPA,12); 
    DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_ClrBit(E_GPA,14); // GPA14 = Red,   0 : on, 1 : off
		DrvSYS_Delay(speed);
		//DrvSYS_Delay(90000000);
		DrvGPIO_SetBit(E_GPA,12);
		DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_SetBit(E_GPA,14); 
		PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
		DrvSYS_Delay(speed/2); 

		break;
			
			
			case 1:         //green led
			// set RGBled to Green
		PWM_Freq(PWM_PIN,tone_g,duty_cycle); // set PWM output
    DrvGPIO_SetBit(E_GPA,12); 
    DrvGPIO_ClrBit(E_GPA,13); // GPA13 = Green, 0 : on, 1 : off
    DrvGPIO_SetBit(E_GPA,14); 
		DrvSYS_Delay(speed);	
		//DrvSYS_Delay(90000000);
    DrvGPIO_SetBit(E_GPA,12);
		DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_SetBit(E_GPA,14);
		PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output
		DrvSYS_Delay(speed/2);  			
		break;
			
			case 2:        // blue led
				// set RGBled to Blue
		PWM_Freq(PWM_PIN,tone_b,duty_cycle); // set PWM output
    DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
    DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_SetBit(E_GPA,14); 
		DrvSYS_Delay(speed); 
		//DrvSYS_Delay(90000000);
		DrvGPIO_SetBit(E_GPA,12);
		DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_SetBit(E_GPA,14);
		PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output			
		DrvSYS_Delay(speed/2); 
		break;			
			
			case 3:       // white led
			// set RGBled to white
		PWM_Freq(PWM_PIN,tone_w,duty_cycle); // set PWM output
    DrvGPIO_ClrBit(E_GPA,12); // GPA12 = Blue,  0 : on, 1 : off
    DrvGPIO_ClrBit(E_GPA,13); 
    DrvGPIO_ClrBit(E_GPA,14); 
		DrvSYS_Delay(speed); 
		//DrvSYS_Delay(90000000);
		DrvGPIO_SetBit(E_GPA,12);
		DrvGPIO_SetBit(E_GPA,13); 
    DrvGPIO_SetBit(E_GPA,14);
		PWM_Freq(PWM_PIN,0,duty_cycle); // set PWM output			
		DrvSYS_Delay(speed/2); 
		break;
			
		}
	}
}

void Init_LED()						 // Set all LED's as output and turn off
{
	// initialize GPIO pins
	DrvGPIO_Open(E_GPA, 12, E_IO_OUTPUT); // GPA12 pin set to output mode
	DrvGPIO_Open(E_GPA, 13, E_IO_OUTPUT); // GPA13 pin set to output mode
	DrvGPIO_Open(E_GPA, 14, E_IO_OUTPUT); // GPA14 pin set to output mode
	// set GPIO pins output Hi to disable LEDs
	DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Hi to turn off Blue  LED
	DrvGPIO_SetBit(E_GPA, 13); // GPA13 pin output Hi to turn off Green LED
	DrvGPIO_SetBit(E_GPA, 14); // GPA14 pin output Hi to turn off Red   LED
}    

void Init_LED2()           // Initial GPIO pins (GPA 12,13,14) to Output mode  

{
	// initialize GPIO pins
	DrvGPIO_Open(E_GPA, 12, E_IO_OUTPUT); // GPA12 pin set to output mode
	DrvGPIO_Open(E_GPA, 13, E_IO_OUTPUT); // GPA13 pin set to output mode
	DrvGPIO_Open(E_GPA, 14, E_IO_OUTPUT); // GPA14 pin set to output mode
	DrvGPIO_Open(E_GPB, 11, E_IO_OUTPUT); // initial GPIO pin GPB11 for controlling Buzzer
	// set GPIO pins output Hi to disable LEDs
	DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Hi to turn off Blue  LED
	DrvGPIO_SetBit(E_GPA, 13); // GPA13 pin output Hi to turn off Green LED
	DrvGPIO_SetBit(E_GPA, 14); // GPA14 pin output Hi to turn off Red   LED
}    
