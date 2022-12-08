/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);
void initDisplay();
void sevenSegWrite(int digit);
unsigned int readADC();
void initTmp();
void delay_us(unsigned int us);

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initDisplay();
    initTmp();
}

void initTmp() {
    ANSELBbits.ANSB13 = 1;          // RB13 als Input 
    TRISBbits.TRISB13 = 1;
    AD1CHSbits.CH0NA = 1;           // VRef
    AD1CHSbits.CH0SA = 8;
    AD1CON3bits.ADCS = 8;          // Konvertierung
    AD1CON1bits.ON = 1;
}

unsigned int readADC() {
    AD1CON1bits.SAMP = 1;           //Start Sampling
    delay_us(1000);
    AD1CON1bits.SAMP = 0;           // Ende Sampling
    while(AD1CON1bits.DONE == 0);   // Warten, bis Konvertierung fertig
    return ADC1BUF0;
}

void loop() { 
    int valorem = 0;
    unsigned int rawValorem = 0;
    int i = 0;
    
    while(1) {    
        if(i%10 == 0) {
            rawValorem = readADC();
            // -50 + temp * 100 * 3.3 / (2 ^ 12)
            valorem = (3.3/4096)*rawValorem*100 - 40;
            i = 0;
        }
        i++;
        PORTBbits.RB1 = 0;
        PORTBbits.RB2 = 1;
        sevenSegWrite(valorem%10);
        delay_us(5000);

        PORTBbits.RB2 = 0;
        PORTBbits.RB1 = 1;
        sevenSegWrite((valorem/10)%10);
        delay_us(5000);
    }
}

int main(void) {
    setup();
    loop();
}