#include <msp430.h> 

/*
 * main.c
 */
int i=0;
int main(void) {

    //16Mhz
    if (CALBC1_16MHZ==0xFF)
    {
    	while(1);
	}
	DCOCTL = 0;
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	WDTCTL = WDTPW + WDTHOLD;	// Stop watchdog timers

	BCSCTL1 |= DIVA_0;
	P1DIR |= 0X01;
	CCTL0 = CCIE;
	CCR0=32767;
	TACTL = TASSEL_1 + ID_0 + MC_1;
	 _BIS_SR(LPM0_bits + GIE);


	
	//return 0;
}
// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	P1SEL |= BIT0;
	P1SEL2 &= ~BIT0;

    CCR0 += 32767 ;
}

