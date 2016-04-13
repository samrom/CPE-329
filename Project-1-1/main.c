/*
 * Authors: Samantha Romano and Edith Rodriguez
 * Professor Gerfen CPE329-01 SPRING 2016
 * Project 1, 4/8/16
 */

#include <msp430g2553.h>
 volatile char smiley[] = {0x00, 0x01, 0x0A, 0x00,0x11, 0x1F, 0x00, 0x00};
 void lcd_write_cmd(char cmd);
 void lcd_write_data(char data);
 void lcd_write_address(char address);
 void lcd_initialization();
 void lcd_word(char word[]);


/*

 *  LCD Starup Sequence

 *  Power On -> Wait 20ms -> Set Function -> Wait 37us ->

 *  Display Set -> Wait 37us -> Display Clear -> Wait 1.52ms

 *  -> OK

 */
#define ARRAYSIZE 12


int main(void) {


    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    //16Mhz
    if (CALBC1_16MHZ==0xFF) {
	while(1);
	}
	DCOCTL = 0;
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

    P1DIR |= 0xF0; // sets port 1 upper pins as an output and lower as input
    P2DIR  = 0x07; // sets E, RW, and RS as output 0000 0111
    // Interrupts
    P1DIR |= BIT0 + BIT4; // P1.4 and P1.0 output bits for toggling
    P1OUT |= BIT0 + BIT4; // Start with LEDs illuminated

     lcd_initialization();
     char word[] = "Hello World!";
     lcd_word(word);

     //INTERRUPT BEGINS

	 P1IE  |= BIT3 + BIT2; // Enable interrupts for P1.3 and P1.2
	 P1REN |= BIT3 + BIT2; // Add int. pullup/pulldown resistor to P1.3 and P1.2
	 P1OUT |= BIT3 + BIT2; // Config int. resistors for pullup operation
	 P1IES |= BIT3 + BIT2; // Select high to low edge Interrupt on P1.3 and P1.2
	 P1IFG &= ~(BIT3 + BIT2); // Clear the interrupt flags to ensure system
	 _BIS_SR(LPM4_bits + GIE); // Let's Go (to Sleep) in Low Power Mode 4 with
	 // interrupts enabled!

	 return 0;
}

void lcd_word(char word[]){
int i=0;
char address = 0x00;
	while(word[i]){
		lcd_write_address(address++);
		lcd_write_data(word[i++]);
		//address++;
	}
	return;
}
void lcd_initialization(){
	_delay_cycles(6400000); // 40 ms delay
    P2OUT |= 0x01;          // set enable high
    P1OUT = BIT5;  		    // nibble mode
    P2OUT &= ~0x01; 	    // toggle enable off.
    P1OUT &= ~BIT5;
    P2OUT &= ~0x07;
    _delay_cycles(16);
    lcd_write_cmd(0x2F);    //function set
    _delay_cycles(7200);    // 39+ us delay
    lcd_write_cmd(0x0F);    // display set 0000 1111
    _delay_cycles(7200);    // 37 us delay
    lcd_write_cmd(0x01);    // display clear
    _delay_cycles(34000);   // 1.53 + extra ms
    lcd_write_cmd(0x06);    // entry set
    _delay_cycles(34000);   // 1.52ms
    P2OUT ^= 0x01; 		    // set enable low.

}
void lcd_write_cmd(char cmd){
	P2OUT |= 0x01; // set enable high
	//Top Bit
	P1OUT = cmd & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(16); // delay 1us.
	P2OUT ^= 0x01; // set enable off
	_delay_cycles(16);
    P2OUT ^= 0x01; // enable high
	// bottom bit
    _delay_cycles(16);
	P1OUT = (cmd << 4)  & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(16);
	P2OUT ^= 0x01; // enable low
	//Reset
	P2OUT &= ~0x07;
	//P1OUT = 0x00;
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
	_delay_cycles(16); // delay 1us.
	P2OUT ^= 0x01; // set enable off
	_delay_cycles(16);
	P2OUT ^= 0x01; // set enable high
	_delay_cycles(16);
	// Bottom bit
	P1OUT = (data<<4) & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(16);
	P2OUT ^= 0x01; // set enable off
	//Clear everything again.
	_delay_cycles(16);
	P2OUT &= ~0x07;  // sets everything low
//	P1OUT &= 0x00;
    return;

}

void lcd_write_address(char address){
	 lcd_write_cmd(address | 0x80);  // address on the LCD
	 _delay_cycles(800); // 37+ us delay
	return;
}


/* ********************************
 * *********INTERRUPT ROUTINE******
 **********************************/

#pragma vector=PORT1_VECTOR // Interrupt vector
__interrupt void Port_1(void){ // ISR
	 lcd_initialization();
	 P1OUT ^= BIT0;
	 if (~(P1IFG & BIT3)){
		 char omg[] = "    OMG!!!!";
		 lcd_word(omg);
         P1IFG &= ~BIT3; // Clear the Bit 3 interrupt flag,

	 }
	 if (P1IFG & BIT2){
	    P1OUT ^= BIT2; // Toggle P1.1
	    P1IFG &= ~BIT2; // Clear the Bit 2 interrupt flag,
	  }
}
