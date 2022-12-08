/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

void setup() {   
    ANSELAbits.ANSA1 = 0;       // wichtig: Pin darf nicht analog sein, damit es mit digitalen Modulen zusammenarbeiten kann
    TRISCbits.TRISC9 = 0;     // set bit 3 of Port D for output
    TRISBbits.TRISB7 = 0;
    TRISAbits.TRISA1 = 1;
    

    // Init Timer
    T1CONbits.TCKPS0 = 1;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler
    PR1 = 20000;
    T1CONbits.ON = 1;       // turn on Timer1
    
    // Init Interrupt
    IPC2bits.T1IP = 1;  // interrupt enable
    IEC0bits.T1IE = 1;  // interrupt enable
    __builtin_enable_interrupts();
}

void initInputCapture() {
    
    CCP1CON1bits.ON = 0;
    CCP1CON1bits.CCSEL = 1; // Input capture mode
    CCP1CON1bits.CLKSEL = 0;    // sysclk
    CCP1CON1bits.T32 = 0;   // 16bit Mode
    CCP1CON1bits.MOD = 0;   // nicht 3 verwenden, bei 3 lässt sich der Postscaler nicht setzen
    CCP1CON2bits.ICS = 0;   // capture source 0
    CCP1CON1bits.OPS = 1;   // output postscale - alle 2 Events
    CCP1CON1bits.TMRPS = 0; // timer prescaler - 1:1 (24 MHz)
    CCP1PR = 10000;         // timer period register
    RPINR2bits.ICM1R = 2;   // icm auf RA1 (RP2) schalten)

    // Init Interrupt
    
    IPC7bits.CCP1IP = 1;    // interrupt priority - CCP1 Event
    IEC0bits.CCP1IE = 1;    // interrupt enable
    
    //IPC7bits.CCT1IP = 2;
    //IEC0bits.CCT1IE = 1;
    
    CCP1CON1bits.ON = 1;
    __builtin_enable_interrupts();
}

void __ISR(_TIMER_1_VECTOR, IPL1SOFT) Timer1ISR(void){
    LATBbits.LATB7 = !LATBbits.LATB7;
    TMR1 = 0;               // reset timer
    IFS0bits.T1IF = 0;
}

int first = 0;
void __ISR(_CCP1_VECTOR, IPL1SOFT) _CCP1_VECTOR_Handler(void)
{
    if (first == 0) {
        first++;
    } else {
        LATCbits.LATC9 = !LATCbits.LATC9;
        //CCP1TMR = 0;               // reset timer

        int a = 1;
    }
    IFS0bits.CCP1IF = 0;
}

int main(void) {
    setup();
    initInputCapture();
    while(1){
        //LATCbits.LATC9 = 1;
        /*
        if(IFS0bits.CCP1IF == 0) {
            LATCbits.LATC9 = 1;
        } else {
            LATCbits.LATC9 = 0;
        }
        */
        
    }
}