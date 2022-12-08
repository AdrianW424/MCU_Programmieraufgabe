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
    ANSELAbits.ANSA0 = 0;
    TRISAbits.TRISA0 = 1;
    
    
    CCP2CON1bits.ON = 0;
    CCP2CON1bits.CCSEL = 1; // Input capture mode
    CCP2CON1bits.CLKSEL = 0;    // sysclk
    CCP2CON1bits.T32 = 0;   // 16bit Mode
    CCP2CON1bits.MOD = 0;   // nicht 3 verwenden, bei 3 lässt sich der Postscaler nicht setzen
    CCP2CON2bits.ICS = 0;   // capture source 0
    CCP2CON1bits.OPS = 1;   // output postscale - alle 2 Events
    CCP2CON1bits.TMRPS = 3; // timer prescaler - 1:16 (24 MHz)
	// Prescaler von 16 wichtig, damit für einen Überlauf 43,69ms benötigt werden
	// Der Ultraschallsensor soll maximal 38ms brauchen (wenn kein Hinderniss)
	// damit kann verhindert werden, dass Überlauf stattfindet, ansonsten wäre Rechnung komplizierter
	
    //CCP1PR = 10000;         // timer period register (vielleicht)
    RPINR2bits.ICM2R = 1;   // icm auf RA1 (RP2) schalten)

    // Init Interrupt
    
    IPC7bits.CCP2IP = 1;    // interrupt priority - CCP1 Event
    IEC0bits.CCP2IE = 1;    // interrupt enable
    
    //IPC7bits.CCT1IP = 2;
    //IEC0bits.CCT1IE = 1;
    
    CCP2CON1bits.ON = 1;
    __builtin_enable_interrupts();
}

void initOutputCompare() {
    //ANSELAbits.ANSA2 = 0;
    TRISAbits.TRISA1 = 0;
    
    // für 10us Impuls bei 24MHz -> (10us/(1/24MHz) = 240 Takte)
    // Signal muss 240 Takte High sein
    // danach beliebige Anzahl an Low
    // in diesem Fall: 9*240 Takte = 2160 Takte Low
    // insgesamt damit 2400 Takte
    // PR = 2400
    // CMPA = 0
    // CMPB = 240 // bzw. 239
    
    CCP1PRbits.PR = 32000;
    CCP1RAbits.CMPA = 5000;
    CCP1RBbits.CMPB = 700;
    CCP1CON1bits.MOD = 0b0101;
    CCP1CON2bits.OCFEN = 1;
    CCP1CON1bits.ON = 1;
}

void delay_us(unsigned int us) {
    _CP0_SET_COUNT(0);        //set core timer to zero
    us *= 1000000;
    while (us > _CP0_GET_COUNT()) {};
}

void __ISR(_TIMER_1_VECTOR, IPL1SOFT) Timer1ISR(void){

    LATBbits.LATB7 = !LATBbits.LATB7;
    TMR1 = 0;               // reset timer
    IFS0bits.T1IF = 0;
}

void __ISR(_CCP2_VECTOR, IPL1SOFT) _CCP2_VECTOR_Handler(void)
{
    // vielleicht in Assembler
    if(CCP2STATbits.ICBNE == 1) {
        int a = CCP2BUF;    // erster Bufferwert (FIFO) - kleinerer Wert
        a = CCP2BUF;    // zweiter Bufferwert (FIFO) - größerer Wert
        //CCP1TMR = 0;               // reset timer
        if(CCP2STATbits.ICBNE == 0) {   // wenn beide Bufferwerte ausgelesen wurden, Flag ICBNE ist 0
            LATCbits.LATC9 = !LATCbits.LATC9;
        }
        CCP2TMR = 0;               // reset timer
    }
    IFS0bits.CCP2IF = 0;
    delay_us(1);
}

int main(void) {
    setup();
    initInputCapture();
    initOutputCompare();
    /*
    int val = 0;
    while(1){
        if (val >= 1000) {
            val = 0;
        }
        CCP1RAbits.CMPA = val;
        val++;
    }
    */
}