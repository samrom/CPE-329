#include <msp430.h> 
#include <stdio.h>
#include <string.h>


 void lcd_write_cmd(char cmd);
 void lcd_write_data(char data);
 void lcd_write_address(char address);
 void lcd_initialization();
 void lcd_word(char word[]);
/*
 * main.c
 * This is Assingment 5 using crystal oscillator
 */

#define mSec 16000
#define uSec 16

#define cycles 32768
int seconds = 0;
int minutes = 0;
int hours = 0;
char address = 0x00;

 void main(void)
 {

	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	  if (CALBC1_16MHZ==0xFF) {
		  while(1);
		}
	  DCOCTL = 0;
	  BCSCTL1 = CALBC1_16MHZ;
	  DCOCTL = CALDCO_16MHZ;


 	P1SEL |= BIT0;
 	P1SEL2 &= ~BIT0;





    P1DIR |= 0xF0 + BIT0; // sets port 1 upper pins as an output and lower as input
    P2DIR  = 0x07; // sets E, RW, and RS as output 0000 0111
    // Interrupts
    BCSCTL3 = 0;

	CCTL0 = CCIE;					// CCR0 interrupt enabled
	CCR0 = cycles;					// 512 -> 1 sec, 30720 -> 1 min
	TACTL = TASSEL_1 + MC_2;			// ACLK, cONTMODE


	_BIS_SR(LPM0_bits + GIE);			// Enter LPM3 w/ interrupt ?? LPM0 or LMP3?


     lcd_initialization();
    // lcd_write_cmd(0x01);    // display clear
      //_delay_cycles(34000);   // 1.53 + extra ms

     //INTERRUPT BEGINS
	 return 0;
}

void lcd_word(char word[]){
int i=0;

	while(word[i]){
		lcd_write_address(address++);
		lcd_write_data(word[i++]);
	}
	return;
}
void lcd_initialization(){
	_delay_cycles(6400000); // 40 ms delay
    P2OUT |= 0x01;          // set enable high
    P1OUT = BIT5;  		    // nibble mode
    _delay_cycles(uSec);
    P2OUT &= ~0x01; 	    // toggle enable off.
    _delay_cycles(mSec);
    P1OUT &= ~BIT5;
    P2OUT &= ~0x07;
    _delay_cycles(uSec);
    lcd_write_cmd(0x2F);    //function set
    _delay_cycles(7200);    // 39+ us delay
    lcd_write_cmd(0x0F);    // display set 0000 1111
    _delay_cycles(7200);    // 37 us delay
    lcd_write_cmd(0x01);    // display clear
    _delay_cycles(34000);   // 1.53 + extra ms
    lcd_write_cmd(0x06);    // entry set
    _delay_cycles(34000);   // 1.52ms
    P2OUT ^= 0x01; 		    // set enable low.
    _delay_cycles(7200);    // 37 us delay
}
void lcd_write_cmd(char cmd){
	P2OUT |= 0x01; // set enable high
	//Top Bit
	P1OUT = cmd & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(uSec); // delay 1us.
	P2OUT ^= 0x01; // set enable off
	_delay_cycles(uSec);
    P2OUT ^= 0x01; // enable high
	// bottom bit
    _delay_cycles(uSec);
	P1OUT = (cmd << 4)  & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(uSec);
	P2OUT ^= 0x01; // enable low
	//Reset
	P2OUT &= ~0x07;

	_delay_cycles(7200); // 37 us delay
    return;
}

void lcd_write_data(char data){
    //Setup

	P2OUT |= BIT2; // set RS high
    //_delay_cycles(16);
	P2OUT |= 0x01; // set enable high
	//Top Bit
	P1OUT =  data & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(uSec); // delay 1us.
	P2OUT ^= 0x01; // set enable off
	_delay_cycles(uSec);
	P2OUT ^= 0x01; // set enable high
	_delay_cycles(uSec);
	// Bottom bit
	P1OUT = (data<<4) & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(uSec);
	P2OUT ^= 0x01; // set enable off
	//Clear everything again.
	_delay_cycles(uSec);
	P2OUT &= ~0x07;  // sets everything low
	_delay_cycles(7200); //added delay to finaly work
//	P1OUT &= 0x00;
    return;

}

void lcd_write_address(char address){
	 lcd_write_cmd(address | 0x80);  // address on the LCD
	 _delay_cycles(800); // 37+ us delay
	return;
}
 // Timer A0 interrupt service routine
 #pragma vector=TIMER0_A0_VECTOR
 __interrupt void Timer_A (void)
 {
   char OutString[20];
   sprintf(OutString, "%02d:%02d:%02d ", hours, minutes, seconds);


	P1OUT |= BIT0;
	CCR0 += cycles;
	if(seconds < 59){
		seconds++;
	}
	else{
		seconds = 0;
		minutes++;
	}
	if(minutes == 59){
		minutes = 0;
		hours++;
	}

	lcd_word(OutString);
    address = 0x00;
}
