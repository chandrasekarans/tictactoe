//--- Program Header ---
/*----------------------------------------------------------------------------------------------
	File Name:	ticTacToe.c
	Author:		Chandrasekaran Srinivasan
	Date:		10/01/2024
	Modified:	None

	Description: This program shows the working of a tictactoe game using push buttons and Leds using PIC18F45K22.
----------------------------------------------------------------------------------------------*/

// Preprocessor ------------------------------------------------------------------------------

#include "pragmas.h"
#include <adc.h>
#include <stdlib.h>
#include <delays.h>
#include <stdio.h>
#include <p18f45k22.h>
#include "usart.h"



// Constants ---------------------------------------------------------------------------------

#define TRUE		1
#define	FALSE		0	

#define ROW 3
#define COL 3
#define TIMER  2
#define ARRAYSIZE 3
#define SECONDS 60
#define MINUTES 60
#define T0FLAG INTCONbits.TMR0IF
#define TURN ttt.ttt.playerTurn

#define XPLAYS00 (row == 0 && col == 0)&&TURN&&(!VISITED[row][col])
#define OPLAYS00 (row == 0 && col == 0)&&(!TURN)&&(!VISITED[row][col])
#define LED_00_OP TRISAbits.TRISA7
#define LED_00_ON LATAbits.LATA7

#define XPLAYS01 (row == 0 && col == 1)&&TURN&&(!VISITED[row][col])
#define OPLAYS01 (row == 0 && col == 1)&&(!TURN)&&(!VISITED[row][col])
#define LED_01_OP TRISAbits.TRISA6
#define LED_01_ON LATAbits.LATA6

#define XPLAYS02 (row == 0 && col == 2)&&TURN&&(!VISITED[row][col])
#define OPLAYS02 (row == 0 && col == 2)&&(!TURN)&&(!VISITED[row][col])
#define LED_02_OP TRISCbits.TRISC0
#define LED_02_ON LATCbits.LATC0


#define XPLAYS10 (row == 1 && col == 0)&&TURN&&(!VISITED[row][col])
#define OPLAYS10 (row == 1 && col == 0)&&(!TURN)&&(!VISITED[row][col])
#define LED_10_OP TRISCbits.TRISC1
#define LED_10_ON LATCbits.LATC1

#define XPLAYS11 (row == 1 && col == 1)&&TURN&&(!VISITED[row][col])
#define OPLAYS11 (row == 1 && col == 1)&&(!TURN)&&(!VISITED[row][col])
#define LED_11_OP TRISCbits.TRISC2
#define LED_11_ON LATCbits.LATC2

#define XPLAYS12 (row == 1 && col == 2)&&TURN&&(!VISITED[row][col])
#define OPLAYS12 (row == 1 && col == 2)&&(!TURN)&&(!VISITED[row][col])
#define LED_12_OP TRISCbits.TRISC3
#define LED_12_ON LATCbits.LATC3

#define XPLAYS20 (row == 2 && col == 0)&&TURN&&(!VISITED[row][col])
#define OPLAYS20 (row == 2 && col == 0)&&(!TURN)&&(!VISITED[row][col])
#define LED_20_OP TRISDbits.TRISD0
#define LED_20_ON LATDbits.LATD0

#define XPLAYS21 (row == 2 && col == 1)&&TURN&&(!VISITED[row][col])
#define OPLAYS21 (row == 2 && col == 1)&&(!TURN)&&(!VISITED[row][col])
#define LED_21_OP TRISDbits.TRISD1
#define LED_21_ON LATDbits.LATD1

#define XPLAYS22 (row == 2 && col == 2)&&TURN&&(!VISITED[row][col])
#define OPLAYS22 (row == 2 && col == 2)&&(!TURN)&&(!VISITED[row][col])
#define LED_22_OP TRISDbits.TRISD2
#define LED_22_ON LATDbits.LATD2

#define TURNCOUNTER ttt.ttt.turnCounter
#define SEL ttt.select
#define MAXREACHED SEL >= 3
#define ROLLBACKTO0 SEL = 0;
#define MINREACHED SEL <= -1
#define ROLLBACKTO2 SEL = 2;

#define PBMASK 0x0F
#define PBS (PORTA&PBMASK)
#define NOPRESS 0x0F

#define SEC ttt.time[1]
#define MAXSEC SECONDS <= ttt.time[1]
#define MIN ttt.time[0]
#define MAXMIN MINUTES <= ttt.time[0]
#define VISITED ttt.ttt.visited
#define TICTACTOE ttt.ttt.ticTacToe


// Global Variables --------------------------------------------------------------------------
int row = 0, col =0, count = 0;
char pbState = FALSE;
int secCount = 0;
char resetFlag = FALSE;
char stopTimer = FALSE;
typedef struct game    // structure of game board
{
	char ticTacToe[ROW][COL];
	char visited[ROW][COL];
	char playerTurn;
	int turnCounter;
	char flag;
}game_t;

typedef struct tttSystem    // structure of ttt system
{
	char address[4];
	game_t ttt;
	int select;
	int time[TIMER];
	char status;
} tttSystem_t;

tttSystem_t ttt;



// Functions ---------------------------------------------------------------------------------
/*--- initializeTttSystem ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function initializes the variables and an array in the tictactoe system.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/


void initializeTttSystem(void)
{
	int i=0,j=0;
	sprintf(ttt.address,"111");
	TURN = TRUE;
	SEL = 0;			//0 means row, 1 means col
	MIN = 0;			//minutes 00:00
	SEC = 0;			//Seconds 00:00
	TURNCOUNTER= 0;
	ttt.status = 1;
	ttt.ttt.flag= 0;
	resetFlag = FALSE;
	stopTimer = FALSE;
	for(i=0; i< ROW; i++)
	{
		for(j=0; j<COL; j++)
		{
			TICTACTOE[i][j]= 0xFF;
			VISITED[i][j]= FALSE;
		}
	}
} // eo initializeTttSystem



/*--- functionName ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function  sets the oscilator frequency to 4 MHZ
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void setOsc4MHz(void)
{	
	OSCCON= 0x52;		 // Sleep on slp cmd, HFINT is 4MHz,  Internal oscillator block 				
	OSCCON2 = 0x04;			//  Clock is from Oscillator, MF off, Secondary  Oscillator id off, Primary Oscillator		
	OSCTUNE = 0x80;		 // PLL is  disabled, Default factory frequency tuning	is used			
	
	while (OSCCONbits.HFIOFS != 1); 	
}

/*--- configT0 ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function  configures the timer0.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/

void configT0(void)
{
	T0CON=0x93;
	TMR0H=0x0B;
	TMR0L=0xDC;
	T0FLAG=FALSE;
}// eo configT0

/*--- resetT0 --------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function  resets the timer0.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void resetT0(void)
{
	T0FLAG=FALSE;
	TMR0H=0x0B;
	TMR0L=0xDC;
} // eo resetT0

/*--- portConfig ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function deals with pickit3 port configuration. All the leds are configured as high impedence by assigning 0xFF to TRISC and TRIS D.
 			In the high impedence state leds won't glow.Later on in the display function each of the leds are multiplexed by assigning 0 & 1 (turning on and off).
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void portConfig(void)
{
	// Port A is configured as digital output. In RA6 and RA7 the leds for position 00 and 01 are connected.
	ANSELA=0x00;
	LATA=0x00;
	TRISA=0xFF;

	ANSELB=0x00;
	LATB=0x00;
	TRISB=0xFF;

	// Port C is configured as digital output.
	ANSELC=0x00;
	LATC=0x00;   // From RC0 to RC3 leds to show position 02 through 12 is connected.
	TRISC=0xFF;

	//Port D digital output.
	ANSELD=0x00;// led is connected to RD0 to RD2 for rest of the positions 
	LATD=0x00;
	TRISD=0xFF;
} // eo portConfig


/*--- serialportConfig ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function deals with pickit3 Serialport configuration
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void serialportConfig(void)
{

	SPBRG1=25;            
	RCSTA1=0x90;
	TXSTA1=0x26;
	BAUDCON1=0x40;
} // eo serialportConfig

/*--- initializeSystem ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function initializes the system by calling all configuration functions. 
			This function is called inside the main function.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/

void initializeSystem(void)
{
	serialportConfig();
    setOsc4MHz();
	configT0();
	portConfig(); 
	initializeTttSystem();
	printf("Game on");
} // eo initializeSystem

/*--- checkwin ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function will return the game status.
			IT RETURNS 1 FOR GAME IS OVER WITH RESULT
			IT RETURNS -1 AND SHOWS GAME IS IN PROGRESS
			IT  RETURNS O TO SHOW GAME IS OVER AND NO RESULT
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/

int checkwin(void)
{
    if (TICTACTOE[0][0] == TICTACTOE[0][1] && TICTACTOE[0][1] == TICTACTOE[0][2])		//checks whether the values in the Row1 matches
	{
		if(TICTACTOE[0][1] != 0xFF)
        	return 1;
	}
    else if (TICTACTOE[1][0] == TICTACTOE[1][1] && TICTACTOE[1][1] == TICTACTOE[1][2])		//checks whether the values in the Row2 matches
	{	
		if(TICTACTOE[1][1] != 0xFF)
        	return 1;
	}
    else if (TICTACTOE[2][0] == TICTACTOE[2][1] && TICTACTOE[2][1] == TICTACTOE[2][2])		//checks whether the values in the Row3 matches
    {
		if(TICTACTOE[2][1] != 0xFF)
        	return 1;
	}
    else if (TICTACTOE[0][0] == TICTACTOE[1][0] && TICTACTOE[1][0] == TICTACTOE[2][0])		//checks whether the values in the Col1 matches
	{
		if(TICTACTOE[1][0] != 0xFF)
        	return 1;
	}
    else if (TICTACTOE[0][1] == TICTACTOE[1][1] && TICTACTOE[1][1] == TICTACTOE[2][1])		//checks whether the values in the Col2 matches
	{
		if(TICTACTOE[1][1] != 0xFF)
        	return 1;
	}	
    else if (TICTACTOE[0][2] == TICTACTOE[1][2] && TICTACTOE[1][2] == TICTACTOE[2][2])		//checks whether the values in the Col3 matches
	{
		if(TICTACTOE[1][2] != 0xFF)
        	return 1;
		if((TICTACTOE[0][2]&TICTACTOE[1][2]&TICTACTOE[2][2]) == 'X')
			return 1;
	}
    else if (TICTACTOE[0][0] == TICTACTOE[1][1] && TICTACTOE[1][1] == TICTACTOE[2][2])		// checks whether the values matches dioganally. (Cross1)
	{
		if(TICTACTOE[1][1] != 0xFF)
        	return 1;
	}
    else if (TICTACTOE[0][2] == TICTACTOE[1][1] && TICTACTOE[1][1] == TICTACTOE[2][0])		//checks whether the values matches dioganally (Cross2)
	{
		if(TICTACTOE[1][1] != 0xFF)
        	return 1;
	}
    else if (TURNCOUNTER == 9)
        return 0;					
    else
        return  -1;				
} // eo checkwin
/*----ledOperation----------------------------------------------------------------------------

/*--- initiateTimer ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/01/2024
Modified:	Name or None
Desc:		This function initiates the timer and for that the variables for seconds and minutes are incremented accordingly.To keep count on seconds and minutes 
			one array called timer is used.If these variables are incremented above 60 they are initialized back to 0.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void initiateTimer(void)
{
	if(!stopTimer) //if game is still playing then only initiate the timer
	{
		SEC++;
		if(MAXSEC)
		{
			SEC = FALSE;
			MIN++;			//minutes is incremented from 1 to 60. After that it is initialized to 0.
			if(MAXMIN)	//when 59 minutes done
			{
				SEC = FALSE;		//reset seconds to 0
				MIN = FALSE;		//reset minutes to 0
			}
		}
	}
}// eo initiateTimer

//--------------------------------------------------------------------------------------------*/
void ledOperation(void)
{
	// green led shows player X and Red led shows Player 
	if(XPLAYS00)  // it checks the conditions when player X is playing and the leds are turned on accordingly.
	{
		LED_00_OP&=0;		// leds are set a output.
		LED_00_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}		
	if(OPLAYS00) // it checks the conditions when player 0 is playing and the leds are turned on accordingly.
	{
		LED_00_OP&=0;
		LED_00_ON|=1;		//led1 off led2 on
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS01)
	{
		LED_01_OP&=0;
		LED_01_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS01)
	{
		LED_01_OP&=0;
		LED_01_ON|=1;		//led1 off led2 on
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS02)
	{
		LED_02_OP&=0;
		LED_02_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS02)
	{
		LED_02_OP&=0;
		LED_02_ON|=1;		//led1 off led2 on
		VISITED[row][col] = TRUE;
	}
    if(XPLAYS10)
	{
		LED_10_OP&=0;
		LED_10_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS10)
	{
		LED_10_OP&=0;
		LED_10_ON|=1;		//led1 off led2 on
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS11)
	{
		LED_11_OP&=0;
		LED_11_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS11)
	{
		LED_11_OP&=0;
		LED_11_ON|=1;		//led1 off led2 on
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS12)
	{
		LED_12_OP&=0;
		LED_12_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS12)
	{
		LED_12_OP&=0;
		LED_12_ON|=1;		//led1 off led2 off
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS20)
	{
		LED_20_OP&=0;
		LED_20_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS20)
	{
		LED_20_OP&=0;
		LED_20_ON|=1;		//led1 off led2 off
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS21)
	{
		LED_21_OP&=0;
		LED_21_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS21)
	{
		LED_21_OP&=0;
		LED_21_ON|=1;		//led1 off led2 off
		VISITED[row][col] = TRUE;
	}
	if(XPLAYS22)
	{
		LED_22_OP&=0;
		LED_22_ON&=0;		//led1 on led2 off
		VISITED[row][col] = TRUE;
	}
	if(OPLAYS22)
	{
		LED_22_OP&=0;
		LED_22_ON|=1;		//led1 off led2 off
		VISITED[row][col] = TRUE;
	}
}
/*--- tttBoardDisplay ---------------------------------------------------------------------------

--------------------------------------------------------------------------------------------*/
void tttBoardDisplay(void)
{
	printf("\e[K\e[12;5HR");
	printf("\e[K\e[14;5HW");
	printf("\e[K\e[8;0H\r\t");
	printf("\e[K\e[9;0H\r\t | | ");
	printf("\e[K\e[10;6H 0");
   	printf("\e[K\e[10;0H\r\t%c|%c|%c", TICTACTOE[0][0], TICTACTOE[0][1], TICTACTOE[0][2]);

    printf("\e[K\e[11;0H\r\t_|_|_");
    printf("\e[K\e[12;0H\r\t | | ");
	printf("\e[K\e[13;4H O 1");
    printf("\e[K\e[13;0H\r\t%c|%c|%c", TICTACTOE[1][0], TICTACTOE[1][1], TICTACTOE[1][2]);
	
    printf("\e[K\e[14;0H\r\t_|_|_");
    printf("\e[K\e[15;0H\r\t | | ");
	printf("\e[K\e[16;6H 2");
	printf("\e[K\e[16;0H\r\t%c|%c|%c", TICTACTOE[2][0], TICTACTOE[2][1], TICTACTOE[2][2]);
	
	printf("\e[K\e[17;0H\r\t | | ");
}
/*--- display ---------------------------------------------------------------------------
Author:		Csrinivasan
Date:		10/06/2020
Modified:	Name or None
Desc:		This function displays which row/col is enetered,the game time,turn count as well as the game board.
			The conditions for detecting pushbutton actions are written in this function. In this function the visited array is used to 
			make sure no move is made in the same position. Everytime when one move is placed in a position one flag is made true.
Input: 		none
Returns:	none
--------------------------------------------------------------------------------------------*/
void display(void)
{
	int i,j;
	char data;
	if(ttt.status)			//ready flag is true
	{
		SEC=0;
		MIN=0;
	}
	printf("\e[K\e[0;0HTicTacToe%s\t",ttt.address);
	printf("Game Time: %02d:%02d\t",MIN,SEC);
	if(ttt.status)
		printf("Status: Ready");
	else if(resetFlag)
		printf("Status: Reset");
	else if(checkwin() == 1)
	{
		stopTimer = TRUE;
		if(TURN)
			printf("Status: O Won  ");
		else
			printf("Status: X Won  ");
	}
	else
		printf("Status: Playing");
	if(resetFlag)	//Reset reinitialize all
	{
		initializeTttSystem();	//reinitialize
		portConfig();			//all led will off
		printf("\e[0J");		//clear screen
	}
	if(!TURN)
		printf("\e[\e[2;0HPlayer:O \t\t Turn Count:%d",TURNCOUNTER);
	else
		printf("\e[\e[2;0HPlayer:X \t\t Turn Count:%d",TURNCOUNTER);
	printf("\e[K\e[4;0H\rSelect: %d",SEL);
	printf("\e[K\e[6;0H\r\tCOLUMN ");
	printf("\e[K\e[7;0H\r\t0 1 2");
	if(PBS != NOPRESS)
	{
		pbState = PBS;
		if(pbState == 0x0E)			//Push button for increment is connected to RA0
		{
			SEL++;
			if(MAXREACHED)
			{
				ROLLBACKTO0
			}
			ttt.status = FALSE;
			while(PBS != NOPRESS);
		}
		else if(pbState == 0x0D)			// Pushbutton for decrement is connected to RA1
		{
			SEL--;
			if(MINREACHED)
			{
				ROLLBACKTO2
			}
			ttt.status = FALSE;
			while(PBS != NOPRESS);
		}
		else if(pbState == 0x03)			// Enter and mode pressed together
		{
			//mode
			secCount++;
			if(secCount>=2)
			{
				resetFlag = TRUE;
				secCount = 0;
			}
		}
		else if(pbState == 0x0B)			//Pushbutton for enter is connected to RA2
		{
			count++;
			ttt.status = FALSE;
			if(count == 1)
			{
				row = SEL;
				printf("\e[K\e[3;0H\rRow Entered");
			}
			else if(count == 2)
			{
				col = SEL;
				printf("\e[K\e[3;0H\rColumn entered");
			}
			else
			{
				count = 0;
				if(TURN)
				{
					if(!VISITED[row][col])
					{
						TICTACTOE[row][col] = 'X';
						TURN = !TURN;
						TURNCOUNTER++;
						printf("\e[K\e[21;0H               \r");
					}
					else
					{
						printf("\e[K\e[21;0HAlready Visited");
					}
				}
				else
				{
					if(!VISITED[row][col])
					{
						TICTACTOE[row][col] = 'O';
						TURN = !TURN;
						TURNCOUNTER++;
						printf("\e[K\e[21;0H               \r");
					}
					else
					{
						printf("\e[K\e[21;0HAlready Visited");
					}
				}
				ledOperation();
			}
			//while(PBS != NOPRESS);
		}
		else
		{
			
		}
	}
	tttBoardDisplay();
}// eo display

/*--- MAIN FUNCTION -------------------------------------------------------------------------
-------------------------------------------------------------------------------------------*/

void main(void)
{
	initializeSystem();
	while(TRUE)
	{
		if(T0FLAG)
		{
			display();    
			resetT0();
			initiateTimer();
		}
	}
} // eo main
