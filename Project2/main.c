/*
 * Project 2: Function Generator
 * Proffesor Gerfen; CPE329-01
 * Authors: Samantha Romano and Edith Rodriguez
 * Submission Date: April 25th, 2016
 */



#include <msp430g2553.h>
#define steps 50 // 60us per step for 16MHz .

#define Vpp  2200
#define Voff 0
#define limit 130

#define voltage 40  // voltage per step
#define bruteCRR0 368




void Drive_DAC(unsigned int level);
volatile unsigned int level = Voff;
int wav_sel = 1;
int duty_sel = 0;
int freq_sel = 0;
int Freq = 0;
int count = 0;
int ccro = 0;
double duty = 0.0;
int sin_sel = 0;


// Look up Tables
int freq[5] = {9500,5000,3300,2500,2000}; // 100Hz, 200Hz, 300Hz, 400Hz, 500Hz;  period/SMclk
double duty_cycle[9] = { .1, .2, .3, .4, .5, .6, .7, .8, .9};
int  ccro_ramp[5]={368, 190, 128, 97, 76}; // CCRO
int ccro_sin[5]={800,400, 266, 200, 160};
int sinCount=0;
const int sin_table[200] = {2087,	2150,	2213,	2277,	2339,	2402,	2464,	2526,	2587,
		2648,	2708,	2768,	2826,	2884,	2941,	2998,	3053,	3107,	3160,	3212,
		3263,	3313,	3361,	3408,	3453,	3498,	3540,	3582,	3621,	3660,	3696,
		3731,	3764,	3796,	3826,	3853,	3880,	3904,	3926,	3947,	3966,	3982,
		3997,	4010,	4021,	4030,	4037,	4042,	4045,	4046,	4045,	4042,	4037,
		4030,	4021,	4010,	3997,	3982,	3966,	3947,	3926,	3904,	3880,	3853,
		3826,	3796,	3764,	3731,	3696,	3660,	3621,	3582,	3540,	3498,	3453,
		3408,	3361,	3313,	3263,	3212,	3160,	3107,	3053,	2998,	2941,	2884,
		2826,	2768,	2708,	2648,	2587,	2526,	2464,	2402,	2339,	2277,	2213,
		2150,	2087,	2023,	1959,	1896,	1833,	1769,	1707,	1644,	1582,	1520,
		1459,	1398,	1338,	1278,	1220,	1162,	1105,	1048,	993,	939,	886,
		834,	783,	733,	685,	638,	593,	548,	506,	464,	425,	386,
		350,	315,	282,	250,	220,	193,	166,	142,	120,	99,	    80,
		64,	    49,	    36,	    25,	    16,	    9,	    4,	    1,	    0,	    1,	    4,
		9,		16,		25,		36,		49,		64,		80,		99,		120,	142,	166,
		193,	220,	250,	282,	315,	350,	386,	425,	464,	506,	548,
		593,	638,	685,	733,	783,	834,	886,	939,	993,	1048,	1105,
		1162,	1220,	1278,	1338,	1398,	1459,	1520,	1582,	1644,	1707,	1769,
		1833,	1896,	1959,	2023};




// PUt const before sin look up table





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
  __delay_cycles(20);               // Delay 20 16 MHz SMCLK periods
                                     // (12.5 us) to allow SIMO to complete
  P1OUT |= BIT4;                     // Set P1.4   (drive /CS high on DAC)
  return;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	if(wav_sel == 1){


			 if(level == Voff){
				  level = Voff +Vpp;
				  Drive_DAC(level);
				  duty = duty_cycle[duty_sel];
				  Freq=freq[freq_sel];
				  Freq=Freq*duty;
				  ccro = Freq;
				  CCR0 += ccro;

				}
			 else{
				  level = Voff;
				  Drive_DAC(level);
				  duty = 1 - duty_cycle[duty_sel];
				  Freq=freq[freq_sel];
				  Freq=Freq*duty;
				  ccro = Freq;
				  CCR0 += ccro;
				}
	 }

	// Sawtooth
	if(wav_sel == 2){
		  if(count < 50){
			  level += voltage;
			  Drive_DAC(level);
			  CCR0 += ccro_ramp[freq_sel];
			  count++;
		  }
		  else{
			  level = Voff;
			  Drive_DAC(level);
			  CCR0 += ccro_ramp[freq_sel];
			  count = 0;
		  }
	}

	// Sin Wave
    if(wav_sel == 3){
    	if(sinCount < 199)
    	{
    		level=sin_table[sinCount];
    		Drive_DAC(level);
    		CCR0=ccro_sin[freq_sel];
    		sinCount++;
    	}else{
    		sinCount=0;
    	}

    }




}
#pragma vector=PORT1_VECTOR // Interrupt vector
__interrupt void Port_1(void){ // ISR
        // Button 1 pressed - change waveforms
		if(P1IFG & BIT1){
			P1IFG &= ~(BIT1);
			if(wav_sel > 2)
			{
				wav_sel = 0;
			}
			wav_sel++;
			_delay_cycles(72000); // delay 1ms for debouncing
		}
		// Button 2 pressed - change frequency
		if(P1IFG & BIT2)
		{
			P1OUT ^= BIT6;
			P1IFG &= ~BIT2;
			if(freq_sel > 3)
			{
				freq_sel = -1;
				ccro=0;
			}
			freq_sel++;
			_delay_cycles(72000); // delay 1ms for debouncing

		}
		// Button 3 pressed - duty cycle change
		if(P1IFG & BIT3){
			P1IFG &= ~BIT3;
			if(duty_sel > 9)
			{
				duty_sel = 0;
				ccro = 0;
				duty=0;
				Freq=0;
				freq_sel=0;
			}
			else{
				duty_sel++;
			}

			_delay_cycles(72000); // delay 1ms for debouncing
		}
		//_delay_cycles(90000);
}

