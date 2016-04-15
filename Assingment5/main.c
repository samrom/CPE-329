#include <msp430.h> 

/*
 * main.c
 * This is Assingment 5 using crystal oscillator
 */


#define cycles 32768

 void main(void)
 {

	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	//P1DIR |= BIT6;

 	P1DIR |= BIT0 +BIT6;					// set P1.0 (LED1) as output
 	P1SEL |= BIT0;
 	P1SEL2 &= ~BIT0;

 	BCSCTL3 |= XCAP_3;

 	CCTL0 = CCIE;					// CCR0 interrupt enabled
 	CCR0 = cycles;					// 512 -> 1 sec, 30720 -> 1 min
 	TACTL = TASSEL_1 + MC_2;			// ACLK, cONTMODE


 	_BIS_SR(LPM0_bits + GIE);			// Enter LPM3 w/ interrupt ?? LPM0 or LMP3?

 }

 // Timer A0 interrupt service routine
 #pragma vector=TIMER0_A0_VECTOR
 __interrupt void Timer_A (void)
 {

	P1OUT ^= BIT6;
	CCR0 += cycles;


 }
