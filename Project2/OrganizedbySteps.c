/* bcompG2211: This code demonstrates the basic function of the Comparator A+
 * module in the MSP430.  Flashes an LED (P1_0) when voltage is above 1/2 Vcc
 * on analog input CA1 (P1_1).
 */

#include <msp430g2553.h>

//#define LED1	BIT0
//#define AIN1	BIT1
//#define AIN2    BIT2

#define P1_0    BIT0
#define P1_1    BIT1
#define P1_2    BIT2
#define P1_3    BIT3
#define P1_4    BIT4
#define P1_5    BIT5
#define P1_6    BIT6
#define P1_7    BIT7

#define P2_0    BIT0
#define P2_1    BIT1
#define P2_2    BIT2
#define P2_3    BIT3
#define P2_4    BIT4
#define P2_5    BIT5
#define P2_6    BIT6
#define P2_7    BIT7



/*  Function Definitions  */

int countingRamp();
void Drive_DAC(unsigned int level);

/*  Global Variables  */
char flash = 0;					// start with no flash on LED
char analog[256];
int  index = 0;
int firstFind=0;
int storeFirst=0;
unsigned int level = 1000;
double VDD = 3.3;


void main(void) {

	WDTCTL = WDTPW + WDTHOLD;	// disable watchdog

	// 16MHz initilization
	if (CALBC1_1MHZ==0xFF) 		    // If calibration constant erased
	{
	while(1);					    // do not load, trap CPU!!
	}
	DCOCTL = 0;					    // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;		    // Set range
	DCOCTL = CALDCO_1MHZ;			// Set DCO step + modulation
	TACTL = TASSEL_2 + ID_0+ MC_2 + TACLR;  // SMCLK, continuous mode, div 4, clear timer
    // end of Clock initilization

	// CCR0 initilization
	CCTL0 = CCIE;                   // CCR0 interrupt enabled
    CCR0 = 110; 					// default starting value
    TACCTL0 = CCIE;     		    // Enable interrupts for CCR0.
	// end of CCR0 initilization

	// Port Initilization
    P1DIR |= P1_0 + P1_3 + P1_4 + P1_6 + P1_7;                  // set CA0, SDI, CS, and CAOUT as an output.
    P1DIR &= ~P1_1; //input for wavegen
    P2DIR |= P2_0 + P2_1 + P2_2 + P2_3 + P2_4 + P2_5 + P2_6;    // set Resistor Network Ports
   // P1DIR &= ~(P1_1 + P1_2);   				                  // set CA1 and CA2 as an input
    P2SEL &= ~(P2_7+P2_6);
    P2SEL2 &= ~(P2_7+P2_6);
    P2OUT=0;
    // end Port Initlization

//    SPI Initilization (from Project 2)

    P1SEL  = BIT7 + BIT5;   // These two lines dedicate P1_7 and P1_5
    P1SEL2 = BIT7 + BIT5;   // for UCB0SIMO and UCB0CLK respectively
//
//    // SPI Setup
//    // clock inactive state = low,
//    // MSB first, 8-bit SPI master,
//    // 4-pin active Low STE, synchronous
//    //
//    // 4-bit mode disabled for now
    UCB0CTL0 |= UCCKPL + UCMSB + UCMST + /* UCMODE_2 */ + UCSYNC;


    UCB0CTL1 |= UCSSEL_2;             // UCB0 will use SMCLK as the basis for
    UCB0BR0 |=0x00;
    UCB0BR1 |= 0x00;

    // the SPI bit clock
    UCB0CTL1 &= ~UCSWRST;             // **Initialize USCI state machine**
     	                              //   SPI now Waiting for something to
     	                               //  be placed in TXBUF.
   // End of SPI Initilization

 //   CACTL1 = CAON + CAIES + CAIE;	// turn on comparator

//    P1REN |= P1_7 + P1_5;


        firstFind=0;
		// Step 1 - setup P2SEL for comparator muxes and short transmission gate (tg)
        CACTL1 |= CAON;
		CACTL2 &= ~(P2CA2+P2CA3); // inputs for CA1
		CACTL2 |= P2CA1; // inputs for CA1 and short
		CACTL2  |= CASHORT;
		CACTL2 &= ~(P2CA4); // input for CA0
		CACTL2 |= P2CA0; // input for CA0

		// Step 2 - Output Function Generator to Capacitor and wait to let it store its value
		P1OUT |= P1_0; // output to capacitor.
		_delay_cycles(1600);

		// Step 3 - Close Vin and switch to Ramp input: P2SEL changes and open TG
		CACTL2 &= ~(P2CA1 + P2CA3+ CASHORT); // for CA2 and open TG
		CACTL2 |= P2CA2;     // select CA2 for P2OUT into Mux

		// Step 4 - Turn on Comparator and select CAEX mux value
		CACTL1 &= ~CAEX;     // select CAEX for mux into comparator
		//CACTL1 |= CAON;

		// Step 5 - Delay for Tau, turn on RC filter is optional
		_delay_cycles(1600); // tau

        level = countingRamp();
        Drive_DAC(level);

} // main


/*  Interrupt Service Routines  */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void){
	Drive_DAC(level);

}

int countingRamp(){

		while(P2OUT++ < 256){




			// Step 6 - Check CAOUT if we found a match.
				if ((CACTL2 & CAOUT)==0x01) {
					//P1OUT |= P1_0;

					firstFind++;
					storeFirst=P2OUT;
				//	storeFirst = (storeFirst*3.3)/256;
					P1OUT &= ~P1_6;
					Drive_DAC(storeFirst);
				}
				else{
					P1OUT &= ~P1_0;
					_delay_cycles(100);
				}
		}
	P2OUT = 0;
	return storeFirst;
}

void Drive_DAC(unsigned int level){
  unsigned int DAC_Word = 0;

  DAC_Word = (0x3000) | (level & 0x0FFF);   // 0x1000 sets DAC for Write
                                            // to DAC, Gain = 1, /SHDN = 1
                                            // and put 12-bit level value
                                            // in low 12 bits.

  P1OUT &= ~BIT4;                    // Clear P1_4 (drive /CS low on DAC)
                                     // Using a port output to do this for now

  UCB0TXBUF = (DAC_Word >> 8);       // Shift upper byte of DAC_Word
                                     // 8-bits to right

  while (!(IFG2 & UCB0TXIFG));       // USCI_A0 TX buffer ready?

  UCB0TXBUF = (unsigned char)
		       (DAC_Word & 0x00FF);  // Transmit lower byte to DAC

  while (!(IFG2 & UCB0TXIFG));       // USCI_A0 TX buffer ready?
  __delay_cycles(200);               // Delay 20 16 MHz SMCLK periods
                                     // (12.5 us) to allow SIMO to complete
  P1OUT |= BIT4;                     // Set P1_4   (drive /CS high on DAC)
  return;
}
