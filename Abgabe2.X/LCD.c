/*
Dokumentenname: LCD.c
Autoren: Jannik Peplau, Adrian Waldera
Matrikelnummern: 1995581, 5932553
Projektname: Labor-Programmieraufgabe
*/

/* some useful functions for controlling LCD */

#define I2C_ADDRESS             0x7C    // 0x3E << 1
#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;

void delay_us(unsigned int us);

// Starts the I2C module
void startI2C() {
    // enable
    I2C3CONbits.SEN = 1;
    // wait until command was executed
    while (IFS2bits.I2C3MIF == 0);
    // Command executed, reset interrupt flag
    IFS2bits.I2C3MIF = 0;
}

// Stops the I2C module
void stopI2C() {
    I2C3CONbits.PEN = 1;
    // wait until command was executed
    while (IFS2bits.I2C3MIF == 0);
    // Command executed, reset interrupt flag
    IFS2bits.I2C3MIF = 0;
} 

// Write address to address bus
u8 writeI2C(u8 byte) {
    // send address to slave and reset
    I2C3TRN = byte | 0;              
    // wait until command was executed
    while (IFS2bits.I2C3MIF == 0);
    // Command executed, reset interrupt flag
    IFS2bits.I2C3MIF = 0;
    return 1;
}

u8 sendI2C(u8 instr, u8 value) {
	u8 ack;
	
	startI2C();
	ack = writeI2C(I2C_ADDRESS);
	//send instr and value only if address was acknowledged
	if (ack == 1) {
        ack = writeI2C(instr);
		ack = writeI2C(value);
    }
	stopI2C();
    delay_us(30); 		// delay required after each instruction
	return ack;
}

void initLCD() {
	u8 mode = 0x00;      // mode for writing a single instruction
    delay_us(50000);     // additional delay after power-on required
    sendI2C(mode,0x38);  // 8 bit interface, 2 lines, set basic table
    sendI2C(mode,0x39);  // set extended table
    sendI2C(mode,0x14);  // adjust internal oscillator frequency
    sendI2C(mode,0x74);  // set contrast (low part)
    sendI2C(mode,0x54);  // set contrast (high part), enable booster for 3,3V
    sendI2C(mode,0x6F);  // follower control 
    sendI2C(mode,0x0E);  // display on, show cursor
    sendI2C(mode,0x01);  // clear display
	delay_us(1200);      // delay required for clearing display
}

void clearLCD() {
    u8 mode = 0x00;
	sendI2C(mode,0x01);  // clear display
	delay_us(1200);      // required for clearing display
}

void writeLCD (char* str, u32 len) {
	u8 mode = 0x40;
	u32 i;
	if (len > 0) {
        for (i=0; i<len; i++)
            sendI2C(mode,str[i]);
    }
}

void setCursor(u8 row, u8 col) {
	u8 p;
	u8 mode = 0x80;
	
	// set cursor to row 0 or 1 and column 0 to 15
	if (row > 1) row = 0;
	if (col > 15) col = 0;
	if (row == 0) 
		p = 0x80 + col;
	else 
		p = 0xC0 + col;
	sendI2C(mode, (0x00 | p));
}