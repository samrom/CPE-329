#include <msp430.h> 

/*
 * main.c
 */

/*
 * Flow chart from PmodCLP rm RevA.doc - PmodCLP rm RevA
 *  page 3
 *
 *  LCD Starup Sequence
 *  Power On -> Wait 20ms -> Set Function -> Wait 37us ->
 *  Display Set -> Wait 37us -> Display Clear -> Wait 1.52ms
 *  -> OK
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	//MSP430 Pins
    #define P1.0 '0';
	#define P1.1 '1';
	#define P1.2 '2';
	#define P1.3 '3';
	#define P1.4 '4';
	#define P1.5 '5';
	#define P1.6 '6';
	#define P1.2 '7';


	return 0;
}
void setFunction(int sFunction){
	 __delay_cycles(16000000); // change to 20ms
   }

void setDisplay(int sDisplay) {
	 __delay_cycles(16000000); // change to 37us

   }

void clearDisplay(int cDisplay){
	 __delay_cycles(16000000); // change to 1.52ms
   }
