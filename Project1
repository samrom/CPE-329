#include <msp430g2553.h>

 void lcd_write_cmd(char cmd);

 void lcd_write_data(char data);

 void lcd_write_address(char address);

 void lcd_initialization();
 void Newlcd_initializationDelete();


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

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
   // P1IE |= BIT3; // P1.3 interrupt enabled
 //   P1IES |= BIT3; // P1.3 Hi/lo edge
 //   P1IFG &= ~BIT3; // P1.3 IFG cleared
    //16Mhz

	if (CALBC1_16MHZ==0xFF) {

	while(1);

	}

	DCOCTL = 0;

	  BCSCTL1 = CALBC1_16MHZ;

	  DCOCTL = CALDCO_16MHZ;

//MSP430 Pins

    P1DIR |= 0xF0; // sets port 1 upper pins as an output and lower as input

    P2DIR = 0x07; // sets E, RW, and RS as output 0000 0111
  //  RSTDIR = 0x00;




    //INTERRUPT ENDS


    lcd_initialization();

    lcd_write_address(0x00);  // display set 0000 1111
    lcd_write_data('H');
    _delay_cycles(4);
    lcd_write_address(0x01);
    lcd_write_data('E');
	lcd_write_address( 0x02);
	lcd_write_data('L');
	lcd_write_address( 0x03);
	lcd_write_data('L');
	lcd_write_address( 0x04);
	lcd_write_data('O');
	lcd_write_address( 0x05);

	lcd_write_data(' ');

	lcd_write_address( 0x06);
	lcd_write_data('W');
	lcd_write_address( 0x07);
	lcd_write_data('O');
	lcd_write_address( 0x08);
	lcd_write_data('R');
	lcd_write_address( 0x09);
	lcd_write_data('L');
	lcd_write_address( 0x0A);
	lcd_write_data('D');
	lcd_write_address( 0x0B);
	lcd_write_data('!');


	 //INTERRUPT BEGINS
	    	 P1DIR |= BIT0 + BIT4; // P1.4 and P1.0 output bits for toggling
	    	 P1OUT |= BIT0 + BIT4; // Start with LEDs illuminated
	    	 P1IE |= BIT3 + BIT2; // Enable interrupts for P1.3 and P1.2
	    	 P1REN |= BIT3 + BIT2; // Add int. pullup/pulldown resistor to P1.3 and P1.2
	    	 P1OUT |= BIT3 + BIT2; // Config int. resistors for pullup operation
	    	 P1IES |= BIT3 + BIT2; // Select high to low edge Interrupt on P1.3 and P1.2
	    	 P1IFG &= ~(BIT3 + BIT2); // Clear the interrupt flags to ensure system
	    	 // starts with no interrupts
	    	 // LOW POWER MODE VERSION
	    	 _BIS_SR(LPM4_bits + GIE); // Let's Go (to Sleep) in Low Power Mode 4 with
	    	 // interrupts enabled!


 ////**ENDS LOOP VERSION PUSHBUTTON


	return 0;

}

/*

 * Initialization for 4-Bit mode
 *

 */
#pragma vector=PORT1_VECTOR // Interrupt vector
__interrupt void Port_1(void){ // ISR
 // CHECK AND CLEAR EACH POSSIBLE BUTTON INTERRUPT SEPARATELY
 // NOTE THAT BOTH ARE ALWAYS CHECKED AND NO 'ELSE' IS USED.
 if (P1IFG & BIT2){
 P1OUT ^= BIT0; // Toggle P1.0
 P1IFG &= ~BIT2; // Clear the Bit 2 interrupt flag,
 // leave all other bits untouched
 }
	P1OUT ^=BIT4;
 if (P1IFG & BIT3){
 P1OUT ^= BIT4; // Toggle P1.4
 lcd_initialization();
 lcd_write_address(0x00);  // display set 0000 1111
lcd_write_data(' ');
lcd_write_address(0x01);
lcd_write_data(' ');
lcd_write_address( 0x02);
lcd_write_data(' ');
lcd_write_address( 0x03);
lcd_write_data('*');
lcd_write_address( 0x04);
lcd_write_data('*');
lcd_write_address(0x05);
lcd_write_data('S');
lcd_write_address( 0x06);
lcd_write_data('M');
lcd_write_address( 0x07);
lcd_write_data('I');
lcd_write_address( 0x08);
lcd_write_data('L');
lcd_write_address( 0x09);
lcd_write_data('E');
lcd_write_address( 0x0A);
lcd_write_data('*');
lcd_write_address(0x0B);
lcd_write_data('*');
 P1IFG &= ~BIT3; // Clear the Bit 3 interrupt flag,
 // leave all other bits untouched
 }
 _delay_cycles(6400000); // 40 ms delay
     P2OUT |= 0x01; // set enable high
     P1OUT = BIT5;  // nibble mode
     P2OUT &= ~0x01; // toggle enable off.
     P1OUT &= ~BIT5;

     P2OUT &= ~0x07;
     _delay_cycles(16);
}

void Newlcd_initializationDelete(){
	_delay_cycles(6400000); // 40 ms delay
    P2OUT |= 0x01; // set enable high
    P1OUT = BIT5;  // nibble mode
    P2OUT &= ~0x01; // toggle enable off.
    P1OUT &= ~BIT5;

    P2OUT &= ~0x07;
    _delay_cycles(16);
    lcd_write_cmd(0x2F);
    _delay_cycles(7200); // 39+ us delay
    lcd_write_cmd(0x0F);  // display set 0000 1111
    _delay_cycles(7200);   // 37 us delay
    lcd_write_cmd(0x01);  // display clear
    _delay_cycles(34000); // 1.53 + extra ms
    lcd_write_cmd(0x04);  // entry mode set 0000 0100, decrement mode.
    _delay_cycles(34000); // 1.52ms
    P2OUT ^= 0x01; // set enable low.

}
void lcd_initialization(){

	_delay_cycles(6400000); // 40 ms delay
    P2OUT |= 0x01; // set enable high
    P1OUT = BIT5;  // nibble mode
    P2OUT &= ~0x01; // toggle enable off.
    P1OUT &= ~BIT5;

    P2OUT &= ~0x07;
    _delay_cycles(16);
    lcd_write_cmd(0x2F);
    _delay_cycles(7200); // 39+ us delay
    lcd_write_cmd(0x0F);  // display set 0000 1111
    _delay_cycles(7200);   // 37 us delay
    lcd_write_cmd(0x01);  // display clear
    _delay_cycles(34000); // 1.53 + extra ms
    lcd_write_cmd(0x06);
    _delay_cycles(34000); // 1.52ms
    P2OUT ^= 0x01; // set enable low.


    // button pressed
}
void lcd_write_cmd(char cmd){

	//P2OUT &= ~0x07;
	P2OUT |= 0x01; // set enable high
	//Top Bit
	P1OUT = cmd & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(16); // delay 1us.
	P2OUT ^= 0x01; // set enable off
	_delay_cycles(16);
    P2OUT ^= 0x01; // enable high
	// bottom bit
	P1OUT = (cmd << 4)  & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(16);
	P2OUT ^= 0x01; // enable low

	//Reset
	P2OUT &= ~0x07;
	P1OUT = 0x00;
	_delay_cycles(7200); // 37 us delay
    return;

}

void lcd_write_data(char data){
    //Setup
	//P2OUT &= ~0x07; //clear BIT0, BIT1, BIT2
	P2OUT |= BIT2; // set RS high

	//_delay_cycles(0.64); // 40ns delay
	P2OUT |= 0x01; // set enable high

	//Bottom Bit
	P1OUT =  data & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(4); // delay 1us.

	P2OUT ^= 0x01; // set enable off
	//P2OUT ^= BIT2; // set RS low
//

	_delay_cycles(16);
	P2OUT ^= 0x01; // set enable high

	// Top bit
	P1OUT = (data<<4) & (BIT7 | BIT6 | BIT5 | BIT4);
	_delay_cycles(4);
	P2OUT ^= 0x01; // set enable off
	//P2OUT ^= BIT2;


	//Clear everything again.
	P2OUT &= ~0x07;  // sets everything low
	P1OUT &= 0x00;
    return;

}

void lcd_write_address(char address){
	// P1OUT = 0x00;
	 lcd_write_cmd(address | 0x80);  // address on the LCD
	 _delay_cycles(800); // 37+ us delay
	return;
}
