#include <msp430.h> 

/*
 * main.c
 */
int i;
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer


    P1DIR |= BIT5+BIT4;
// int count=0;
//    while(count<21){
//    P1OUT |=BIT5;
//    __delay_cycles(10000);
//   P1OUT &= ~BIT5;
//    __delay_cycles(10000);
//    count++;


while(1)
{
	for(i=0;i<200;i++)
	{
		   P1OUT |=BIT5;
		    __delay_cycles(10000);
		   P1OUT &= ~BIT5;
		    __delay_cycles(10000);
	}
	for(i=0;i<10;i++)
	{
		P1OUT |=BIT5;
		__delay_cycles(1000000);
	}
}
	
   // P1OUT|=BIT5;
}
