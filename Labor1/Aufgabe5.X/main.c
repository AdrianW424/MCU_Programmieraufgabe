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
    
    CCP1PRbits.PR = 1000;   // Breite der Periode
    CCP1RAbits.CMPA = 0;
    //CCP1RA = (1/10)*CCP1PR;
    CCP1RBbits.CMPB = CCP1PRbits.PR;
    
    CCP1CON1bits.MOD = 0b0101;      // set mode (Buffered Dual-Compare/PWM mode) - damit beide Flanken bestimmbar
    //CCP1CON2bits.OCAEN = 1;         // set output for CCP1-Module
    CCP1CON1bits.ON = 1;            // set start module

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
        
        CCP1RAbits.CMPA = ratio;
        delay_us(10000);
    }
   
}

 

int main(void) {
    setup();
    loop();
}



#include <xc.h>
#include <stdint.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);
void delay_us(unsigned int us);

u8 count = 0;

void initTimer() {
    TRISAbits.TRISA9 = 0;  // RA9 to output
    
    TMR1 = 0;					// clear TMR1 of Timer 1
    PR1 = 2;                    // set PR1 to 1
    T1CONbits.TCKPS0 = 0;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler to 256
    
    // Init Interrupt
    IPC4bits.T1IP = 2;
    IFS0bits.T1IF = 0;  // interrupt flag
    IEC0bits.T1IE = 1;  // interrupt enable
	// => mit Prescaler 1:2 wird f zu 12 MHz, für 500 Hz brauchen wir 12 MHz / (500*32) = 750 für das Periodenregister
    T1CONbits.ON = 1;       // turn on Timer1
    __builtin_enable_interrupts();
}

void initInputCapture() {
    CCP1CON1bits.ON = 0;
    CCP1CON1bits.CCSEL = 1; // Input capture mode
    CCP1CON1bits.CLKSEL = 0;
    CCP1CON1bits.T32 = 0;
    CCP1CON1bits.MOD = 0;   // vielleicht 3
    CCP1CON2bits.ICS = 0; 
    CCP1CON1bits.OPS = 0; 
    CCP1CON1bits.TMRPS = 0;
    CCP1CON1bits.SYNC = 0;
    CCP1PR = 10000;
    RPINR2bits.ICM1R = 2;

    // Init Interrupt
    
    IPC18bits.CCP1IP = 5;
    IPC18bits.CCP1IS = 2;
    IEC2bits.CCP1IE = 1;  // interrupt enable
    IFS2bits.CCP1IF = 0;  // interrupt flag
    
    IPC18bits.CCT1IP = 6;
    IEC2bits.CCT1IE = 1;  // interrupt enable
    IFS2bits.CCT1IF = 0;  // interrupt flag
    
    CCP1CON1bits.ON = 1;
    __builtin_enable_interrupts();
}

void initOutputCompare() {
    //ANSELAbits.ANSA9 = 0;
    TRISAbits.TRISA12 = 0;
    
    CCP1PRbits.PR = 1000;
    CCP1RAbits.CMPA = 0;
    CCP1RBbits.CMPB = 1000;
    CCP1CON1bits.MOD = 0b0101;
    CCP1CON2bits.OCAEN = 1;
    CCP1CON1bits.ON = 1;
    __builtin_enable_interrupts();
}

void __ISR(_CCT1_VECTOR, IPL6SOFT) ICTISR(){
    
    IFS2bits.CCT1IF = 0;      // clear interrupt-flag
}

void __ISR(_CCP1_VECTOR, IPL5SOFT) _ICISR(){
    
    IFS2bits.CCP1IF = 0;      // clear interrupt-flag
}

void __ISR(_TIMER_1_VECTOR, IPL2SOFT) Timer1ISR(){
    if (count == 0) {
        LATAbits.LATA9 = 0;
    } else if (count == 9) {
        LATAbits.LATA9 = 1;
        count = -1;
    }
    count++;
    TMR1 = 0;               // reset timer
    // Reset Timer
    IFS0bits.T1IF = 0;      // clear interrupt-flag
}

u32 readSensor() {
    //CCP1
}

void loop() {
    
}

int main(void) {
    /*
    ANSELA = 0;
    //initTimer();
    initInputCapture();
    int CCPBuffer = 0;
    while(1) {
        if (CCP1STATbits.ICOV == 1) {
            u8 val = CCP1BUF;
            IFS2bits.CCP1IF = 0;  // interrupt flag
            val = CCP1BUF;
            val = CCP1BUF;
            val = CCP1BUF;
            val = CCP1BUF;
            val = CCP1BUF;
            val = CCP1BUF;
            u32 test = val + 1;
            //CCP1STATbits.ICOV = 0;
            //initInputCapture();
        }
    }
    */
    initOutputCompare();
    while(1) {
        
    }
}
