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
    while(i < 10) {
        sevenSegWrite(i);
        delay_us(1000000);
        i++;
        if(i == 10) {
            i = 0;
        }
    }
}

int main(void) {
    setup();
    loop();
}