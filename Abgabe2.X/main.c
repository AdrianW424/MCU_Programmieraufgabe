/*
Dokumentenname: main.c
Matrikelnummern: 1995581, 5932553
Projektname: Labor-Programmieraufgabe
*/

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

/**
 * fuction setup()
 * description: 
 *  - runs all initialization functions
 * parameters: 
 *  - none
 * return:
 *  - none
*/
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

/**
 * fuction initI2C()
 * description:
 *  - initializes the I2C interface
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initI2C() {
    ANSELBbits.ANSB13 = 0;  // disable analog input 
    
    // set ports SDA3=RB7 and SCL3=RB13 as output
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB13 = 0;

    I2C3CON = 0;    // reset bits
    
    I2C3CONbits.A10M = 0;   // 7-bit slave address
    
    I2C3CONbits.DISSLW = 0; // slew rate control disable bit
    
    // PBCLK = 24MHz, BR = 400KHz, TPBOG = 130ns
    // BRG = (PBCLK/(2*BR) - 1 - (PBCLK*TPBOG / 2) = 27,44 -> 27 
    I2C3BRG = 27;
    
    I2C3CONbits.ON = 1; // enable I2C
    I2C3CONbits.I2CEN = 1; // enable
    IFS2bits.I2C3MIF = 0; // unset MIF
}

/**
 * fuction initInputCapture()
 * description:
 *  - initializes input capture
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initInputCapture() {
    TRISBbits.TRISB5 = 1; // set port RB5 as input
    
    CCP2CON1bits.ON = 0;    // disable CCP2
    CCP2CON1bits.CCSEL = 1; // input capture mode
    CCP2CON1bits.CLKSEL = 0;// sysclk
    CCP2CON1bits.T32 = 0;   // 16bit mode
    CCP2CON1bits.MOD = 0;   // edge detect mode
    CCP2CON2bits.ICS = 0;   // capture source 0 => ICM2
    CCP2CON1bits.OPS = 1;   // output postscale - every 2 events
    CCP2CON1bits.TMRPS = 3; // timer prescaler - 1:64 (24 MHz)
    
    RPINR2bits.ICM2R = 11;   // remap ICM2 to RB5 (RP11)

    /*Init Interrupt*/
    IPC19bits.CCP2IP = 1;   // interrupt priority
    IFS2bits.CCP2IF = 0;    // clear interrupt flag
    IEC2bits.CCP2IE = 1;    // interrupt enable
    /****************/
    
    CCP2CON1bits.ON = 1;    // enable CCP2
}

/**
 * fuction initOutputCompare()
 * description:
 *  - initializes output compare
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initOutputCompare() {
    TRISAbits.TRISA1 = 0;   // set port RA1 as output
    
    CCP1PRbits.PR = 24000;  // every 1ms a pulse for 10us
    CCP1RAbits.CMPA = 0;    //
    CCP1RBbits.CMPB = 239;  //
    CCP1CON1bits.MOD = 5;   // dual edge compare mode
    CCP1CON2bits.OCFEN = 1; // set to pin OCM1F - RA1
    CCP1CON1bits.ON = 1;    // enable CCP1
}

/**
 * fuction initTimer1()
 * description:
 *  - initializes timer 1
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initTimer1() {
    /*Init Timer*/
    T1CONbits.TCKPS0 = 0;   //
    T1CONbits.TCKPS1 = 1;   // set prescaler to 64
    T1CONbits.ON = 1;       // turn on Timer1
    /************/
    
    /*Init Interrupt*/
    IPC4bits.T1IP = 4;  // interrupt priority
    IFS0bits.T1IF = 0;  // clear interrupt flag
    IEC0bits.T1IE = 1;  // interrupt enable
    /****************/
}

/**
 * fuction initButton1()
 * description:
 *  - initializes button 1
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initButton1() {
    TRISBbits.TRISB9 = 1;   // set port RB9 as input
    
    IEC0bits.INT2IE = 0;    // interrupt disable - clears flags
    INTCONbits.INT2EP = 0;  // interrupt on falling edge
    IPC1bits.INT2IP = 6;    // interrupt priority 
    IFS0bits.INT2IF = 0;    // clear interrupt flag
    IEC0bits.INT2IE = 1;    // interrupt enable
}

/**
 * fuction initButton3()
 * description:
 *  - initializes button 3
 * parameters: 
 *  - none
 * return:
 *  - none
*/
void initButton3() {
    TRISCbits.TRISC4 = 1;   // set port RC4 as input
    
    IEC0bits.INT3IE = 0;    // interrupt disable - clears flags
    INTCONbits.INT3EP = 0;  // interrupt on falling edge
    IPC1bits.INT3IP = 7;    // interrupt priority 
    IFS0bits.INT3IF = 0;    // clear interrupt flag
    IEC0bits.INT3IE = 1;    // interrupt enable
}

/* Variables for global use */
int distance;                       // most recent measurement by sensor
int menu = 0;                       // currently active menu item
int savedDistance = 401;            // saved value for comparison in menu item 3
int diffDistance = 401;             // difference between saved and current value for menu item 3
u8 validFlag = 1;                   // validity of current sensor reading, 0 for valid, 1 for out of range
/****************************/

/**
 * function Button3ISR()
 * description:
 *  - interrupt service routine for when button 3 is pressed
 * parameters
 *  - void
 * return
 *  - 
*/
void __ISR(_EXTERNAL_3_VECTOR, IPL7SOFT) Button3ISR(void) {
    asm volatile(
    "addi $t0, $zero, 2                         \n\t"   // if(menu == 2)
    "bne $t0, %[menu], 1f                       \n\t"   // -||-
    "nop                                        \n\t"
    "addi $t1, %[distance], 0                   \n\t"   // thisDistance = $t1
    "addi $t0, $zero, 401                       \n\t"   // if(thisDistance <= 400) -> (401 would mean invalid reading)
    "beq $t0, $t1, 2f                           \n\t"   // -||-
    "nop                                        \n\t"
    "addi %[validFlag], $zero, 1                \n\t"   // validFlag = 1
    "bne $t0, %[savedDistance], 3f              \n\t"   // if(savedDistance == 401)
    "nop                                        \n\t"
    "addi %[diffDistance], $zero, 401           \n\t"   // diffDistance = 401
    "addi %[savedDistance], $t1, 0              \n\t"   // savedDistance = thisDistance
    "j 1f                                       \n\t"
    "nop                                        \n\t"
    "3:                                             "   // else (savedDistance != 401)
    "sub %[diffDistance], $t1, %[savedDistance] \n\t"   // diffDistance = thisDistance - saveDistance
    "addi %[savedDistance], $zero, 401          \n\t"
    "j 1f                                       \n\t"
    "nop                                        \n\t"
    "2:                                             "   // else (thisDistance > 400 bzw. == 401)
    "addi %[validFlag], $zero, 0                \n\t"   // validFlag = 0
    "1:                                             "
    "addi %[int3if], $zero, 0                   \n\t"   // IF to 0
    :[validFlag]"=r"(validFlag),[savedDistance]"+r"(savedDistance),
            [diffDistance]"+r"(diffDistance),[int3if]"=r"(IFS0bits.INT3IF)
    :[distance]"r"(distance),[menu]"r"(menu)
    :"t0", "t1"
    );
    
    /* Old C code, replaced by assembler code above
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

/**
 * function Button1ISR()
 * description:
 *  - interrupt service routine for when button 1 is pressed
 * paremters:
 *  - void
 * return:
 *  - none
*/
void __ISR(_EXTERNAL_2_VECTOR, IPL6SOFT) Button1ISR(void) {    
    asm volatile(
    "addi %[menu], %[menu], 1   \n\t"   // menu++
    "addi $t0, $zero, 3         \n\t"   // $t0 = 3
    "bne %[menu], $t0, 1f       \n\t"   // goto 1 if menu != 3
    "nop                        \n\t"   
    "addi %[menu], $zero, 0     \n\t"   // menu = 0
    "1:                             "   
    "addi %[int2if], $zero, 0   \n\t"   // set interrupt flag to 0
    :[menu]"+r"(menu),[int2if]"=r"(IFS0bits.INT2IF)
    :
    :"t0"
    );

    /* Old C code, replaced by assembler code above
    menu++;
    if(menu == 3) {
        menu = 0;
    }
    
    IFS0bits.INT2IF = 0;
    */
}

/**
 * function getOpticalDistance()
 * description:
 *  - translate distance reading to optical progress bar
 * paremeters:
 *  - char* optDist distance value as measured by the sensor
 * return:
 *  - none
*/
void getOpticalDistance(char* optDist) {
    char steps = (distance/28)+1;   // there are 14 empty cells on the display -> (400/14 = 28,57 ==> 28); one step for every 28cm; max at 392cm
    optDist[0] = '|';   // first char
    for(int i = 1; i < steps; i++) {    // fill the char array with the desired number of '-'
        optDist[i] = '-';
    }
    
    for(int i = steps; i < 15; i++) {   // fill the char array with the desired number of ' '
        optDist[i] = ' ';
    }
    optDist[15] = '|';  // last char
}

/**
 * function Timer1ISR()
 * description:
 *  - interrupt service for when timer 1 triggers interrupt. This Interrupt is called 5.72 times per second and is used to write or update the LCD display 
 * parameters:
 *  - void
 * return: 
 *  - none
*/
void __ISR(_TIMER_1_VECTOR, IPL4SOFT) Timer1ISR(void) {
    int thisDistance = distance;    // stores current distance value
    char dist[4];                   // stores the distance in an char array (as string)
    if (thisDistance >= 401) {      // the sensor has a measuring range up to 400cm, everything above is displayed with '>'
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
            // 401 will never occur because the maximum measuring range of the sensor is 400cm and everything above will be cut off before. Thus 401 is used as a status number to show that no value has been assigned yet
            if(diffDistance == 401) {   // no second value yet (difference)?
                if(savedDistance == 401) {  // no first value yet?  
                    writeLCD("diff. measure", 13);  // display menu entry
                } else {
                    char diff[3];
                    sprintf(diff, "%0.3d", savedDistance); // int to string
                    writeLCD("saved v1:  ", 11);    // display first saved value
                    writeLCD(diff, 3);              //
                    writeLCD("cm", 2);              //
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
                writeLCD("diff.:   ", 10);  // display difference (value2 - value1)
                writeLCD(diff, 4);          //
                writeLCD("cm", 2);          //
            }
        } else {
            writeLCD("distance invalid", 16);   // if distance >400cm, then there is no accurate value to choose - dont allow saving the value
        }
    } else {
        // menu 0 ==> nothing in second row
        validFlag = 1;
        diffDistance = 401;  // reset measurement
        savedDistance = 401; // reset measurement
    }
    /********************/
    
    TMR1 = 0;               // reset timer
    IFS0bits.T1IF = 0;      // clear interrupt flag
}

/**
 * function readSensor()
 * description:
 *  - reads sensor values from buffer and saves value in global variable 'distance'
 * parameters:
 *  - none
 * return:
 *  - none
*/
void readSensor() {
    int valorem1 = CCP2BUF;     // first buffer value
    int valorem2 = CCP2BUF;     // second buffer value (FIFO)

    if (valorem1 < valorem2) {  // for safety against overflows of the timer register
        int difference = valorem2 - valorem1;  // difference in clock cycles

        /*
        difference = (difference*64)/24; // value in us ==> (difference*64000000)/24000000
        distance = difference/58;   // according to the data sheet - conversion from us to cm
        */
        distance = (difference*64)/1392;
        if(distance > 400) {    // if calculated distance is above 400cm
            distance = 401;     // set the distance to 401 ==> corresponds to ">400cm"
        }
    } else {
        distance = 400;
    }
}

/**
 * function CCP2ISR()
 * description:
 *  - interrupt service routine for 
 * parameters:
 *  - void
 * return:
 *  - none
*/
void __ISR(_CCP2_VECTOR, IPL1SOFT) CCP2ISR(void) {
    readSensor();
    CCP2TMR = 0;                // reset timer
    IFS2bits.CCP2IF = 0;        // clear interrupt flag
}

/**
 * function main()
 * description
 *  - main function, executes the setup() function
 * parameters: 
 * - void
 * return:
 * - int (unreachable due to non-terminating loop)
*/
int main(void) {
    setup();
}