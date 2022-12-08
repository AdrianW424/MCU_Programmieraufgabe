/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

int temp;

void SYSTEM_Initialize(void);

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void){
    TMR1 = 0;               // reset timer
    temp = 1;
    // Reset Timer
    IFS0bits.T1IF = 0;      // clear interrupt-flag
}

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    
    // Init Timer
    T1CONbits.TCKPS0 = 1;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler
    T1CONbits.ON = 1;       // turn on Timer1
    
    TMR2 = 1;
    
    // Init Interrupt
    IPC4bits.T1IP = 5;
    IPC4bits.T1IS = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;  // interrupt enable
    __builtin_enable_interrupts();
}

int main(void) {
    setup();
}