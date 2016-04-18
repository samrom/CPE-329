#include <msp430g2553.h>
#define cycles 4000// 20ms period for 2MHz.
#define Vpp  2200
#define Voff 1120
#define limit 130
#define Vchange 16
#define rampCycles 134

//WaveFunctions
void squareWave();

void Drive_DAC(unsigned int level);
volatile unsigned int level = Voff;
int wav_sel = 0;
int duty_sel = 0;
int freq_sel = 0;
int freq[5] = {10000,5000,3333,2500,2000}; // 100Hz, 200Hz, 300Hz, 400Hz, 500Hz
double duty_cycle[10] = {.1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0};
int ccro = 0;
double duty = 0.0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;        // Stop watchdog timer

  // 16Mhz SMCLK
  if (CALBC1_16MHZ==0xFF)		     // If calibration constant erased
  {
    while(1);                        // do not load, trap CPU!!
  }
  DCOCTL = 0;                        // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_16MHZ;            // Set range
  DCOCTL = CALDCO_16MHZ;             // Set DCO step + modulation
  CCTL0 = CCIE;                      // CCR0 interrupt enabled
  CCR0 = 10000;
  // SMCLK, continuous mode, div 8
  TACTL = TASSEL_2 + ID_3+ MC_2;

  // Init Ports
  P1DIR |= BIT4 +BIT0 + BIT6;                     // Will use BIT4 to activate /CE on the DAC

  P1SEL  = BIT7 + BIT5;  // These two lines dedicate P1.7 and P1.5
  P1SEL2 = BIT7 + BIT5;   // for UCB0SIMO and UCB0CLK respectively

  // SPI Setup
  // clock inactive state = low,
  // MSB first, 8-bit SPI master,
  // 4-pin active Low STE, synchronous
  //
  // 4-bit mode disabled for now
  UCB0CTL0 |= UCCKPL + UCMSB + UCMST + /* UCMODE_2 */ + UCSYNC;


  UCB0CTL1 |= UCSSEL_2;               // UCB0 will use SMCLK as the basis for
                                      // the SPI bit clock

  // Sets SMCLK divider to 16,
  // hence making SPI SCLK equal
  // to SMCLK/1 = 16MHz
  UCB0BR0 |= 0x01;             // (low divider byte) 0001 0000
  UCB0BR1 |= 0x00;             // (high divider byte)


  UCB0CTL1 &= ~UCSWRST;        // **Initialize USCI state machine**
                               // SPI now Waiting for something to
                               // be placed in TXBUF.
  	 P1IE  |= BIT3 + BIT2 + BIT1; // Enable interrupts for P1.3 and P1.0
 	 P1REN |= BIT3 + BIT2 + BIT1; // Add int. pullup/pulldown resistor to P1.3 and P1.0
 	 P1OUT |= BIT3 + BIT2 + BIT1; // Config int. resistors for pullup operation
 	 P1IES |= BIT3 + BIT2 + BIT1; // Select high to low edge Interrupt on P1.3 and P1.0
 	 P1IFG &= ~(BIT3 + BIT2 + BIT1); // Clear the interrupt flags to ensure system

  __enable_interrupt();

} // end of main


void Drive_DAC(unsigned int level){
  unsigned int DAC_Word = 0;

  DAC_Word = (0x3000) | (level & 0x0FFF);   // 0x1000 sets DAC for Write
                                            // to DAC, Gain = 1, /SHDN = 1
                                            // and put 12-bit level value
                                            // in low 12 bits.

  P1OUT &= ~BIT4;                    // Clear P1.4 (drive /CS low on DAC)
                                     // Using a port output to do this for now

  UCB0TXBUF = (DAC_Word >> 8);       // Shift upper byte of DAC_Word
                                     // 8-bits to right

  while (!(IFG2 & UCB0TXIFG));       // USCI_A0 TX buffer ready?

  UCB0TXBUF = (unsigned char)
		       (DAC_Word & 0x00FF);  // Transmit lower byte to DAC

  while (!(IFG2 & UCB0TXIFG));       // USCI_A0 TX buffer ready?
  __delay_cycles(200);               // Delay 200 16 MHz SMCLK periods
                                     // (12.5 us) to allow SIMO to complete
  P1OUT |= BIT4;                     // Set P1.4   (drive /CS high on DAC)
  return;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	if(wav_sel == 1){

		 if(level == Voff){
			  //P1OUT ^= BIT0;
			  level = Voff +Vpp;
			  Drive_DAC(level);
			  duty = duty_cycle[duty_sel];
			  ccro = freq[freq_sel]*duty;
			  CCR0 += ccro;//duty_cycle[duty_sel]);
			}
			else{
			 // P1OUT ^= BIT6;
			  level = Voff;
			  Drive_DAC(level);
			  duty = 1 - duty_cycle[duty_sel];
			  ccro = freq[freq_sel]*duty;
			  CCR0 += ccro;//(100-duty_cycle[duty_sel]);
			}
	 }
	if(wav_sel == 2){
		//P1OUT &= ~BIT6;
	}
    if(wav_sel == 3){
    	//P1OUT |= BIT6;
    }

	 else{
		 //P1OUT |= BIT0;
	 }


}
#pragma vector=PORT1_VECTOR // Interrupt vector
__interrupt void Port_1(void){ // ISR
        // Button 1 pressed
		if(P1IFG & BIT1){
			P1IFG &= ~(BIT1);
			if(wav_sel > 3)
			{
				wav_sel = 0;
			}
			wav_sel++;
		}
		// Button 2 pressed
		if(P1IFG & BIT2)
		{
			P1OUT ^= BIT6;
			P1IFG &= ~BIT2;
			if(freq_sel > 4)
			{
				freq_sel = 0;
			}
			freq_sel++;

		}
		// Button 3 pressed
		if(P1IFG & BIT3){
			P1IFG &= ~BIT3;
			if(duty_sel > 9)
			{
				duty_sel = 0;
			}
			duty_sel++;
		}
}


void squareWave(){
	 // Square Wave
//	if(level == Voff){
//		  P1OUT ^= BIT0;
//		  level = Voff +Vpp;
//		  Drive_DAC(level);
//		  CCR0 += 20000;
//
//	}
//	else{
//		  P1OUT ^= BIT6;
//		  level = Voff;
//		  Drive_DAC(level);
//		  CCR0 += 20000;
//	}
}
