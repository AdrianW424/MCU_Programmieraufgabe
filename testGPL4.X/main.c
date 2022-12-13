/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void initInputCapture();
void initOutputCompare();
void initTimer1();
void initButton1();

void setup() {   
    TRISCbits.TRISC9 = 0;     // set bit 9 of Port C for output
	initInputCapture();
    initOutputCompare();
    initTimer1();
    initButton1();
    
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
    CCP2CON1bits.TMRPS = 3; // timer prescaler - 1:64 (24 MHz)
	// Prescaler von 64 wichtig, damit für einen Überlauf 174,76ms benötigt werden
	// Der Ultraschallsensor soll maximal 38ms brauchen (wenn kein Hindernis)
	// damit kann verhindert werden, dass Überlauf stattfindet, ansonsten wäre Rechnung komplizierter
    // neuer Trigger kommt alle 100us
    // dadurch große Fehlersicherheit, weil Vielfaches der 38ms gewartet werden kann
    
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
    
    // für 10us Impuls bei 24MHz -> (10us/(1/24MHz) = 240 Takte)
    // Signal muss 240 Takte High sein
    // danach beliebige Anzahl an Low
    // in diesem Fall: 9*240 Takte = 2160 Takte Low
    // insgesamt damit 2400 Takte
    // PR = 2400
    // CMPA = 0
    // CMPB = 240 // bzw. 239
    
    CCP1PRbits.PR = 2400;
    CCP1RAbits.CMPA = 0;
    CCP1RBbits.CMPB = 239;
    CCP1CON1bits.MOD = 3;
    CCP1CON2bits.OCFEN = 1;
    CCP1CON1bits.ON = 1;
}

void initTimer1() {
    TRISBbits.TRISB7 = 0;   // just for testing
    
    // Init Timer
    T1CONbits.TCKPS0 = 0;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler to 16
    T1CONbits.ON = 1;       // turn on Timer1
    
    // Init Interrupt
    IPC2bits.T1IP = 4;  // interrupt enable
    IEC0bits.T1IE = 1;  // interrupt enable
}

void initButton1() {
    // Interrupt Pins INT1 is RB14
    ANSELBbits.ANSB14 = 0;
    TRISBbits.TRISB14 = 1;
    TRISBbits.TRISB13 = 0;
    
    IEC0bits.INT1IE = 0;
    // fallende Flanke
    INTCONbits.INT1EP = 0;
    IPC1bits.INT1IP = 6;
    IFS0bits.INT1IF = 0;
    IEC0bits.INT1IE = 1;
}

void delay_us(unsigned int us) {
    _CP0_SET_COUNT(0);        //set core timer to zero
    us *= 1000000;
    while (us > _CP0_GET_COUNT()) {};
}

/*Variables for global use*/
int distance;   // current measured distance
int menu = 0;   // selected menu
/**************************/

void __ISR(_EXTERNAL_1_VECTOR, IPL6SOFT) Button1ISR(void) {
    IEC0bits.INT1IE = 0;
    /*menu++;
    if(menu == 3) {
        menu = 0;
    }*/
    //while(!PORTBbits.RB14);
    
    asm volatile(
    "addi %[menu], %[menu], 1   \n\t"
    "addi $t0, $zero, 3         \n\t"
    "bne %[menu], $t0, 1f       \n\t"
    "nop                        \n\t"
    "addi %[menu], $zero, 0     \n\t"
    "1:                             "
    :[menu]"+r"(menu)
    :
    :"t0"
    );
    
    if (menu == 2) {
        LATBbits.LATB13 = 1;
    } else {
        LATBbits.LATB13 = 0;
    }
    
    IFS0bits.INT1IF = 0;
    IEC0bits.INT1IE = 1;
}

void getOpticalDistance(char* optDist) {
    char steps = (distance/28)+1;
    optDist[0] = '|';
    for(int i = 1; i < steps; i++) {
        optDist[i] = '-';
    }
    
    for(int i = steps; i < 15; i++) {
        optDist[i] = '_';
    }
    optDist[15] = '|';
}

void __ISR(_TIMER_1_VECTOR, IPL4SOFT) Timer1ISR(void){
    // dieser Interrupt wird ca. 5,72 mal pro Sekunde aufgerufen (5-6 mal)
    // dient dazu das LCD-Display zu schreiben/updaten
    int thisDistance = distance;
    char dist[3];
    sprintf(dist, "%d", thisDistance); // int to string
    
    LATBbits.LATB7 = !LATBbits.LATB7;
    
    /*Clears the LCD display*/
    /*
    clearLCD();
    */
    /************************/
    
    /*LCD Prints 1st row*/
    /*
     writeLCD("distance:  ", 11);
     writeLCD(dist, 3);
     writeLCD("cm", 2);
     */
    /********************/
    
    /*LCD Prints 2nd row*/
    /*
     setCursor(1,0);
     if(menu == 1){
        // optical distance
        char optDist[16];
        getOpticalDistance(optDist);
        writeLCD(optDist, 16);
     } else if(menu == 2){
        // difference calculator
        
     }
     // menu 0 ==> nothing in second row
     */
    /********************/
    
    TMR1 = 0;               // reset timer
    IFS0bits.T1IF = 0;
}

void readSensor() {
    int valorem1 = CCP2BUF;    // erster Bufferwert (FIFO) - kleinerer Wert
    int valorem2 = CCP2BUF;    // zweiter Bufferwert (FIFO) - größerer Wert
    // hier sind beide Buffer wieder leer (Flag ICBNE = 0)
    if (valorem1 < valorem2) {  // theoretisch sollte kein Überlauf stattfinden, aber zu Sicherheitszwecken
        int difference = valorem2 - valorem1;  // difference in Takten
        difference = (difference*64)/24; // value in us ==> (difference*64000000)/24000000 ==> Nullen lassen sich kürzen
        distance = difference/58;   // nach Datenblatt Formel zur Umrechnung
        if(difference > 400) {
            distance = 400;
        }
    } else {
        // maximal value
        distance = 400;
    }
}

void __ISR(_CCP2_VECTOR, IPL1SOFT) _CCP2_VECTOR_Handler(void)
{
    // vielleicht in Assembler - lieber nicht (vielleicht nur Teile)
    if(CCP2STATbits.ICBNE == 1) {
        readSensor();
        
        /***** Nur zu Darstellungszwecken *****/
        if(CCP2STATbits.ICBNE == 0) {   // wenn beide Bufferwerte ausgelesen wurden, Flag ICBNE ist 0
            LATCbits.LATC9 = !LATCbits.LATC9;
        }
        /**************************************/
        
        CCP2TMR = 0;               // reset timer
        delay_us(1);
    }
    IFS0bits.CCP2IF = 0;
}

int main(void) {
    setup();
}