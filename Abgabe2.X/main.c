
#include <stdint.h>
#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);
void delay_us(unsigned int us);
u8 writeI2C(u8 byte);
void startI2C();
void stopI2C();
void initLCD();
void clearLCD();
void writeLCD (char* str, u32 len);
void setCursor(u8 row, u8 col);

void initInputCapture();
void initOutputCompare();
void initTimer1();
void initButton1();
void initButton3();
void initI2C();

void setup() {
    SYSTEM_Initialize();
    
    initI2C();
    initLCD();
    initOutputCompare();
    initInputCapture();
    initTimer1();
    initButton1();
    initButton3();
}

void initI2C() {
    // Analoger Eingang des Ports SCL3 deaktivieren
    ANSELBbits.ANSB13 = 0;
    
    // Ports SDA3=RB7 und SCL3=RB13 als Ausgang setzen
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB13 = 0;

    // Zurücksetzen aller Bits
    I2C3CON = 0;
    // 10-bit Slave Address Flag bit -> I2C3ADD register is a 7-bit slave address
    I2C3CONbits.A10M = 0;
    
    // Slew Rate Control Disable bit -> Slew rate control disabled for Standard Speed mode (400 kHz)
    I2C3CONbits.DISSLW = 0;
    
    // PBCLK = 24MHz, BR = 400KHz, TPBOG = 130ns
    // BRG = (PBCLK/(2*BR) - 1 - (PBCLK*TPBOG / 2) = 27,44 -> 27 
    I2C3BRG = 27;
    
    // I2C anschalten
    I2C3CONbits.ON = 1;
    // enable
    I2C3CONbits.I2CEN = 1;
    // unset MIF
    IFS2bits.I2C3MIF = 0;
}

void initInputCapture() {
    //ANSELBbits.ANSB5 = 0;
    TRISBbits.TRISB5 = 1;
    
    
    CCP2CON1bits.ON = 0;    // disable CCP2
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
    RPINR2bits.ICM2R = 11;   // icm auf RB5 (RP11) schalten)

    // Init Interrupt
    
    IPC19bits.CCP2IP = 1;    // interrupt priority - CCP1 Event
    IFS2bits.CCP2IF = 0;    // interrupt flag
    IEC2bits.CCP2IE = 1;    // interrupt enable
    
    CCP2CON1bits.ON = 1;    // enable CCP2
}

void initOutputCompare() {
    //ANSELAbits.ANSA1 = 0;       // wichtig: Pin darf nicht analog sein, damit es mit digitalen Modulen zusammenarbeiten kann
    TRISAbits.TRISA1 = 0;
    
    // für 10us Impuls bei 24MHz -> (10us/(1/24MHz) = 240 Takte)
    // Signal muss 240 Takte High sein
    // danach beliebige Anzahl an Low
    // in diesem Fall: 99*240 Takte = 23760 Takte Low
    // insgesamt damit 24000 Takte
    // PR = 24000
    // CMPA = 0
    // CMPB = 240 // bzw. 239
    
    CCP1PRbits.PR = 24000;
    CCP1RAbits.CMPA = 0;
    CCP1RBbits.CMPB = 239;
    CCP1CON1bits.MOD = 5;
    CCP1CON2bits.OCFEN = 1; // derzeit auf Pin OCM1F - RA1
    CCP1CON1bits.ON = 1;
}

void initTimer1() {
    // Init Timer
    T1CONbits.TCKPS0 = 0;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler to 64
    T1CONbits.ON = 1;       // turn on Timer1
    
    // Init Interrupt
    IPC4bits.T1IP = 4;  // interrupt priority
    IFS0bits.T1IF = 0;  // interrupt flag
    IEC0bits.T1IE = 1;  // interrupt enable
}

void initButton1() {
    // Interrupt Pins INT1 is RB9
    //ANSELBbits.ANSB9 = 0;
    TRISBbits.TRISB9 = 1;
    
    IEC0bits.INT2IE = 0;
    // fallende Flanke
    INTCONbits.INT2EP = 0;
    IPC1bits.INT2IP = 6;
    IFS0bits.INT2IF = 0;
    IEC0bits.INT2IE = 1;
}

void initButton3() {
    // Interrupt Pins INT2 is RC44
    //ANSELCbits.ANSC9 = 0;
    TRISCbits.TRISC4 = 1;
    
    IEC0bits.INT3IE = 0;
    // fallende Flanke
    INTCONbits.INT3EP = 0;
    IPC1bits.INT3IP = 7;
    IFS0bits.INT3IF = 0;
    IEC0bits.INT3IE = 1;
}

/*Variables for global use*/
int distance;   // current measured distance
int menu = 0;   // selected menu
int savedDistance = 401;
int diffDistance = 401;
u8 validFlag = 1; 
/**************************/

void __ISR(_EXTERNAL_3_VECTOR, IPL7SOFT) Button3ISR(void) {
    menu = menu;
    asm volatile(
    "addi $t0, $zero, 2                         \n\t"
    "bne $t0, %[menu], 1f                       \n\t"   // if(menu == 2)
    "nop                                        \n\t"
    "addi $t1, %[distance], 0                   \n\t"   // thisDistance
    "addi $t0, $zero, 401                       \n\t"   // if thisDistance <= 400 (größer kann nur 401 sein)
    "beq $t0, $t1, 2f                           \n\t"   // -||-
    "nop                                        \n\t"
    "addi %[validFlag], $zero, 1                \n\t"   // validFlag = 1
    "bne $t0, %[savedDistance], 3f              \n\t"   // if(savedDistance == 401)
    "nop                                        \n\t"
    "addi %[diffDistance], $zero, 401           \n\t"
    "addi %[savedDistance], $t1, 0              \n\t"
    "j 1f                                       \n\t"
    "nop                                        \n\t"
    "3:                                             "   // else (savedDistance != 401)
    "sub %[diffDistance], $t1, %[savedDistance] \n\t"
    "addi %[savedDistance], $zero, 401          \n\t"
    "j 1f                                       \n\t"
    "nop                                        \n\t"
    "2:                                             "
    "addi %[validFlag], $zero, 0                \n\t"   // validFlag = 0
    "1:                                             "
    "addi %[int3if], $zero, 0                   \n\t"   // IF to 0
    :[validFlag]"=r"(validFlag),[savedDistance]"+r"(savedDistance),
            [diffDistance]"+r"(diffDistance),[int3if]"=r"(IFS0bits.INT3IF)
    :[distance]"r"(distance),[menu]"r"(menu)
    :"t0", "t1"
    );
    
    menu = menu;
    
    /*
    if(menu == 2) {
        int thisDistance = distance;
        if(thisDistance <= 400) {
            validFlag = 1;
            if(savedDistance == 401) {
                diffDistance = 401;
                savedDistance = thisDistance;
            } else {
                diffDistance = thisDistance - savedDistance;
                savedDistance = 401;
            }
        } else {
            validFlag = 0;
        } 
    }
    
    IFS0bits.INT3IF = 0;
    */
}

void __ISR(_EXTERNAL_2_VECTOR, IPL6SOFT) Button1ISR(void) {
    /*
    menu++;
    if(menu == 3) {
        menu = 0;
    }
    
    IFS0bits.INT2IF = 0;
    */
    
    asm volatile(
    "addi %[menu], %[menu], 1   \n\t"
    "addi $t0, $zero, 3         \n\t"
    "bne %[menu], $t0, 1f       \n\t"
    "nop                        \n\t"
    "addi %[menu], $zero, 0     \n\t"
    "1:                             "
    "addi %[int2if], $zero, 0   \n\t"   // IF to 0
    :[menu]"+r"(menu),[int2if]"=r"(IFS0bits.INT2IF)
    :
    :"t0"
    );
}

void getOpticalDistance(char* optDist) {
    char steps = (distance/28)+1;
    optDist[0] = '|';
    for(int i = 1; i < steps; i++) {
        optDist[i] = '-';
    }
    
    for(int i = steps; i < 15; i++) {
        optDist[i] = ' ';
    }
    optDist[15] = '|';
}

void __ISR(_TIMER_1_VECTOR, IPL4SOFT) Timer1ISR(void){
    // dieser Interrupt wird ca. 5,72 mal pro Sekunde aufgerufen (5-6 mal)
    // dient dazu das LCD-Display zu schreiben/updaten
    int thisDistance = distance;
    char dist[4];
    if (thisDistance >= 401) {
        dist[0] = '>';
        thisDistance = 400;
    } else {
        dist[0] = ' ';
    }
    sprintf(&dist[1], "%0.3d", thisDistance); // int to string
    
    /*Clears the LCD display*/
    clearLCD();
    /************************/
    
    /*LCD Prints 1st row*/
    setCursor(0,0);
    writeLCD("distance: ", 10);
    writeLCD(dist, 4);
    writeLCD("cm", 2);
    /********************/
    
    /*LCD Prints 2nd row*/
    setCursor(1,0);
    if(menu == 1){
        // optical distance
        char optDist[16];
        getOpticalDistance(optDist);
        writeLCD(optDist, 16);
    } else if(menu == 2){
        // difference calculator
        if(validFlag) {
            if(diffDistance == 401) {
                if(savedDistance == 401) {
                    writeLCD("diff. measure", 13);
                } else {
                    char diff[3];
                    sprintf(diff, "%0.3d", savedDistance); // int to string
                    writeLCD("saved v1:  ", 11);
                    writeLCD(diff, 3);
                    writeLCD("cm", 2);
                }
            } else {
                char diff[4];
                if(diffDistance < 0) {
                    diff[0] = '-';
                    sprintf(&diff[1], "%0.3d", (diffDistance*(-1))); // int to string
                } else {
                    diff[0] = ' ';
                    sprintf(&diff[1], "%0.3d", diffDistance); // int to string
                }
                writeLCD("diff.:   ", 10);
                writeLCD(diff, 4);
                writeLCD("cm", 2);
            }
        } else {
            writeLCD("distance invalid", 16);
        }
    } else {
        // menu 0 ==> nothing in second row
        validFlag = 1;
        diffDistance = 401;  // reset measurement
        savedDistance = 401; // reset measurement
    }
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
        /*
        difference = (difference*64)/24; // value in us ==> (difference*64000000)/24000000 ==> Nullen lassen sich kürzen
        distance = difference/58;   // nach Datenblatt Formel zur Umrechnung
        */
        distance = (difference*64)/1392;
        if(distance > 400) {
            distance = 401;
        }
    } else {
        // maximal value
        distance = 400;
    }
}

void __ISR(_CCP2_VECTOR, IPL1SOFT) CCP2ISR(void) {
    readSensor();
    CCP2TMR = 0;               // reset timer
    IFS2bits.CCP2IF = 0;
}

int main(void) {
    setup();
}