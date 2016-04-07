#include <msp430g2553.h>

 void lcd_write_cmd(char cmd);

 void lcd_write_data(char data);

 void lcd_write_address(char address);

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

    //16Mhz

	if (CALBC1_16MHZ==0xFF) {

	while(1);

	}

	DCOCTL = 0;

			  BCSCTL1 = CALBC1_16MHZ;

			  DCOCTL = CALDCO_16MHZ;

//MSP430 Pins

    P1DIR = 0x0F; // sets port 1 pins as an output

    P2DIR = 0x07; // sets E, RW, and RS as output 0000 0111


    lcd_initialization();
    lcd_write_address(0x00);  // display set 0000 1111
    lcd_write_data('H');
    lcd_write_address( 0x01);
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
	
	return 0;

}

/*

 * Initialization for 4-Bit mode
 *

 */
void lcd_initialization(){
    _delay_cycles(640000); // 40 ms delay
    P2OUT &= ~0x07;
    P2OUT |= 0x01; // set enable high
    P1OUT = 0x50;
    lcd_write_cmd(0x2F);
    _delay_cycles(700); // 39+ us delay
    lcd_write_cmd(0x0F);  // display set 0000 1111
    _delay_cycles(700);   // 37 us delay
    lcd_write_cmd(0x01);  // display clear
    _delay_cycles(30000); // 1.53 + extra ms
    lcd_write_cmd(0x04);
    _delay_cycles(30000); // 1.52ms
}
void lcd_write_cmd(char cmd){

	P2OUT &= ~0x07;
	P2OUT |= 0x01; // set enable high

	// Top bit
	P1OUT = (cmd << 4) & (BIT7 + BIT6 + BIT5 + BIT4);
	_delay_cycles(4);
	P2OUT &= ~0x07; // reset
	P2OUT |= 0x01; // set enable high again

	//Bottom Bit
	P1OUT = cmd & (BIT3 + BIT2 + BIT1 + BIT0);
	_delay_cycles(4); // delay 1us.

	//Reset
	P2OUT &= ~0x07;
	P1OUT = 0x00;
	_delay_cycles(592); // 37 us delay
    return;

}

void lcd_write_data(char data){
    //Setup
	P2OUT &= ~0x07; //clear BIT0, BIT1, BIT2
	P2OUT |= 0x04; // set RS high
	_delay_cycles(0.64); // 40ns delay
	P2OUT |= 0x01; // set enable high

	// Top bit
	P1OUT = (data << 4) & (BIT7 + BIT6 + BIT5 + BIT4);
	_delay_cycles(4);

	//Setup again.
	P2OUT &= ~0x07; //clear BIT0, BIT1, BIT2
	P2OUT |= 0x04; // set RS high
	_delay_cycles(0.64); // 40ns delay
	P2OUT |= 0x01; // set enable high

	//Bottom Bit
	P1OUT = cmd & (BIT3 + BIT2 + BIT1 + BIT0);
	_delay_cycles(4); // delay 1us.

	//Clear everything again.
	P2OUT &= ~0x07; // sets everything low
	P1OUT &= 0x00;
    return;

}

void lcd_write_address(char address){
	 lcd_write_cmd(BIT7 + address);  // address on the LCD
	 _delay_cycles(592); // 37 us delay
	return;
}