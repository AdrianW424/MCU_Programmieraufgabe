#include <stdint.h>
#include <xc.h>


typedef unsigned char u8;
typedef unsigned int u32;

u8 saw[32] = {0b00000, 0b00001, 0b00010, 0b00011, 0b00100, 0b00101, 0b00110, 0b00111, 0b01000,
            0b01001, 0b01010, 0b01011, 0b01100, 0b01101, 0b01110, 0b01111, 0b10000, 0b10001,
            0b10010, 0b10011, 0b10100, 0b10101, 0b10110, 0b10111, 0b11000, 0b11001, 0b11010,
            0b11011, 0b11100, 0b11101, 0b11110, 0b11111};

//globale Variablen
int position = 0;

void SYSTEM_Initialize(void);
void nextOutput(void);
void delay_us(unsigned int us);

void setup() {
	SYSTEM_Initialize();
	
    TRISBbits.TRISB14 = 0;          // RB14 als Output
    
    // Einstellung ADC
    DAC1CON = 0b11 |			// bit 0-1		: voltage reference source select bits - here it's AVdd
            0b1 << 8 |			// bit 8		: voltage reference output enable bit - output on the CVREF
            0b1 << 15 |			// bit 15		: voltage reference enable bit - enabled voltage reference
            0b11111 << 16; 		// bit 16-20 	: voltage reference selection bits - 31 * AVdd/32
			// bit 16-20 kann auch weggelassen werden -> wird auf max gesetzt (hier)
    
}

void nextOutput(void) {
    DAC1CONbits.DACDAT = saw[position];	// bit 16-20
	position++;
    if(position >= 32) position = 0;
}

void loop() {

   while(1) {
       nextOutput();
       delay_us(62);
   }
}

int main(void) {
    setup();
    loop();
}