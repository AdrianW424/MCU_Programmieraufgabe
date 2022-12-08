/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

int temp;

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void){
    TMR1 = 0;               // reset timer
    
    LATAbits.LATA1 = !LATAbits.LATA1;
    // Reset Timer
    IFS0bits.T1IF = 0;      // clear interrupt-flag
}

void setup() { 
    
    TRISAbits.TRISA1 = 0;     // set bit 14 of Port B to output
    
    // Init Timer
    TMR1 = 0;
    PR1 = 10000;
    T1CONbits.TCKPS0 = 1;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler
    T1CONbits.ON = 1;       // turn on Timer1    
    // Init Interrupt
    IPC2bits.T1IP = 5;
    IPC2bits.T1IS = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;  // interrupt enable
    __builtin_enable_interrupts();
}

int main(void) {
    setup();
}