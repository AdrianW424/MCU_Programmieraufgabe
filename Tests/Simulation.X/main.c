/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);
void delay_us(unsigned int us);

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
	TRISAbits.TRISA12 = 0; // set A12 as Output
    
    CCP2PRbits.PR = 1000;   // Breite der Periode
    CCP2RAbits.CMPA = 0;
    //CCP1RA = (1/10)*CCP1PR;
    CCP2RBbits.CMPB = 100;
    
    CCP2CON1bits.MOD = 0b0101;      // set mode (Buffered Dual-Compare/PWM mode) - damit beide Flanken bestimmbar
    CCP2CON2bits.OCAEN = 1;         // set output for CCP1-Module
    CCP2CON1bits.ON = 1;            // set start module

}

void loop() {
    int ratio = 10;
    int brighter = 1;
    while (1) {
        if (1 == brighter) {
            ratio += 1;
            if (ratio >= 1000)
            {
                brighter = 0;
            }
        }
        else {
            ratio -= 1;
            if (ratio <= 0) {
                brighter = 1;
            }
        }
        
        //CCP2RAbits.CMPA = ratio;
        //delay_us(10000);
    }
   
}

 

int main(void) {
    setup();
    //loop();
    while(1) {
        
    }
}