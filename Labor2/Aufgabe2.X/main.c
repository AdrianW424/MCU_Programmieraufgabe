/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);
void initDisplay();
void sevenSegWrite(int digit);
void delay_us(unsigned int us);

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initDisplay();
}

void loop() { 
	int i = 0;
    int valorem = 0;
    while (i < 1000) {
        PORTBbits.RB1 = 0;
        PORTBbits.RB2 = 1;
        sevenSegWrite(valorem%10);
        delay_us(10000);
        
        PORTBbits.RB2 = 0;
        PORTBbits.RB1 = 1;
        sevenSegWrite((valorem/10)%10);
        delay_us(10000);
        
        if (i%10 == 0) {
            valorem++;
        }
        i++;
        
        if (100 == i) {
            i = 0;
        }
    }
}

int main(void) {
    setup();
    loop();
}