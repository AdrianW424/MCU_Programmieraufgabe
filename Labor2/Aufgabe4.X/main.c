/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

int temp;

void SYSTEM_Initialize(void);
void initDisplay();
void sevenSegWrite(int digit);
int readADC();
void changeDisplay (int number);
void delay_us(unsigned int us);

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
	// Init ADC
    ANSELBbits.ANSB13 = 1;          // B13 als Input 
    TRISBbits.TRISB13 = 1;
    AD1CHSbits.CH0NA = 1;           // VRef
    AD1CHSbits.CH0SA = 8;
    AD1CON3bits.ADCS = 8;          // Konvertierung
    AD1CON1bits.ON = 1;
    
    // Init Timer
    T1CONbits.TCKPS0 = 1;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler
    T1CONbits.ON = 1;       // turn on Timer1
    
    // Init Interrupt
    IEC0bits.T1IE = 1;  // interrupt enable
}

int readADC(){
    AD1CON1bits.SAMP = 1;           //Start Sampling
    delay_us(500);
    AD1CON1bits.SAMP = 0;           // Ende Sampling
    while(0 == AD1CON1bits.DONE);   // Warten, bis Konvertierung fertig
    return ADC1BUF0;
}

void changeDisplay (int number) {
    PORTBbits.RB1 = 0;
    PORTBbits.RB2 = 1;
    sevenSegWrite(number%10);
    delay_us(5000);

    PORTBbits.RB2 = 0;
    PORTBbits.RB1 = 1;
    sevenSegWrite((number/10)%10);
    delay_us(5000);
}

void __ISR(_TIMER_1_VECTOR, IPL1SOFT) Timer1ISR(void){
    
    IEC0bits.T1IE = 0;      // disable interrupt
    
    int adcValue = readADC();
    temp = (3.3/4096)*adcValue*100 - 40; 
    
    TMR1 = 0;               // reset timer
    // Reset Timer
    IFS0bits.T1IF = 0;      // clear interrupt-flag
    IEC0bits.T1IE = 1;      // enable interrupt
}

int main(void) {
    setup();
    initDisplay();
    while(1){
        changeDisplay(temp);
    }