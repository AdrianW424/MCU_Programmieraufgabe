/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>

void setup() {   
    TRISCbits.TRISC9 = 0;     // set bit 3 of Port D for output
    
    __builtin_enable_interrupts();
}

void initInputCapture() {
    ANSELAbits.ANSA0 = 0;
    TRISAbits.TRISA0 = 1;
    
    
    CCP2CON1bits.ON = 0;
    CCP2CON1bits.CCSEL = 1; // Input capture mode
    CCP2CON1bits.CLKSEL = 0;    // sysclk
    CCP2CON1bits.T32 = 0;   // 16bit Mode
    CCP2CON1bits.MOD = 0;   // nicht 3 verwenden, bei 3 l�sst sich der Postscaler nicht setzen
    CCP2CON2bits.ICS = 0;   // capture source 0
    CCP2CON1bits.OPS = 1;   // output postscale - alle 2 Events
    CCP2CON1bits.TMRPS = 3; // timer prescaler - 1:64 (24 MHz)
	// Prescaler von 64 wichtig, damit f�r einen �berlauf 174,48ms ben�tigt werden
	// Der Ultraschallsensor soll maximal 38ms brauchen (wenn kein Hindernis)
	// damit kann verhindert werden, dass �berlauf stattfindet, ansonsten w�re Rechnung komplizierter
    // neuer Trigger kommt alle 100us
    // dadurch gro�e Fehlersicherheit, weil Vielfaches der 38ms gewartet werden kann
    
    //CCP1PR = 10000;         // timer period register (vielleicht)
    RPINR2bits.ICM2R = 1;   // icm auf RA1 (RP2) schalten)

    // Init Interrupt
    
    IPC7bits.CCP2IP = 1;    // interrupt priority - CCP1 Event
    IEC0bits.CCP2IE = 1;    // interrupt enable
    
    CCP2CON1bits.ON = 1;
}

void initOutputCompare() {
    ANSELAbits.ANSA1 = 0;       // wichtig: Pin darf nicht analog sein, damit es mit digitalen Modulen zusammenarbeiten kann
    TRISAbits.TRISA1 = 0;
    
    // f�r 10us Impuls bei 24MHz -> (10us/(1/24MHz) = 240 Takte)
    // Signal muss 240 Takte High sein
    // danach beliebige Anzahl an Low
    // in diesem Fall: 9*240 Takte = 2160 Takte Low
    // insgesamt damit 2400 Takte
    // PR = 2400
    // CMPA = 0
    // CMPB = 240 // bzw. 239
    
    CCP1PRbits.PR = 2399;
    CCP1RAbits.CMPA = 0;
    CCP1RBbits.CMPB = 239;
    CCP1CON1bits.MOD = 3;
    CCP1CON2bits.OCFEN = 1;
    CCP1CON1bits.ON = 1;
}

void delay_us(unsigned int us) {
    _CP0_SET_COUNT(0);        //set core timer to zero
    us *= 1000000;
    while (us > _CP0_GET_COUNT()) {};
}

int difference = 0;

void __ISR(_CCP2_VECTOR, IPL1SOFT) _CCP2_VECTOR_Handler(void)
{
    // vielleicht in Assembler
    if(CCP2STATbits.ICBNE == 1) {
        int valorem1 = CCP2BUF;    // erster Bufferwert (FIFO) - kleinerer Wert
        int valorem2 = CCP2BUF;    // zweiter Bufferwert (FIFO) - gr��erer Wert
        // hier sind beide Buffer wieder leer (Flag ICBNE = 0)
        if (valorem1 < valorem2) {  // theoretisch sollte kein �berlauf stattfinden, aber zu Sicherheitszwecken
            difference = valorem2 - valorem1;
            // difference in us umrechnen
            // us in Distanz umrechnen ==> Distanz = us/58
            
            /***** Nur zu Darstellungszwecken *****/
            if(CCP2STATbits.ICBNE == 0) {   // wenn beide Bufferwerte ausgelesen wurden, Flag ICBNE ist 0
                LATCbits.LATC9 = !LATCbits.LATC9;
            }
            /**************************************/
        } else {
            // maximal distance
        }
        CCP2TMR = 0;               // reset timer
    }
    IFS0bits.CCP2IF = 0;
}

int main(void) {
    setup();
    initInputCapture();
    initOutputCompare();
}