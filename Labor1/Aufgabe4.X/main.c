/*
	Toggle LED with Button
*/

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);
void setAllZero(void);
void setup()
{    
    SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    TRISAbits.TRISA11 = 0;                          //Set A11 for output
    TRISAbits.TRISA12 = 0;                          //Set A12 for output
    TRISAbits.TRISA13 = 0;                          //Set A13 for output
    TRISAbits.TRISA14 = 0;                          //Set A14 for output
    TRISAbits.TRISA15 = 0;                          //Set A15 for output
    
    PR1 = 9375;            // set period register
    T1CONbits.TCKPS0 = 1;   // set prescaler to 256
    T1CONbits.TCKPS1 = 1;
    T1CONbits.ON = 1;       // turn on Timer1
}

void loop() {
    //Turn off LED
    PORTAbits.RA11 = 0;
    PORTAbits.RA12 = 0;
    PORTAbits.RA13 = 0;
    PORTAbits.RA14 = 0;
    PORTAbits.RA15 = 0;
    unsigned int tasterGed = 0;
    unsigned int led = 0;
    unsigned int flag = 1;
    int decFlag = 1;
    //startTimer();
    while (1) { 
        // Ist Taster gedrückt?
        if (0 == PORTBbits.RB9 && flag) {
            flag = 0;
            setAllZero();
            if (0 == tasterGed) {
                tasterGed = 1;
            }
            else {
                tasterGed = 0;
                led = 0;
                setAllZero();
                TMR1 = 0;
            }
        } else if (1 == PORTBbits.RB9) {
            flag = 1;
        }  
        
        // Ist Timer abgelaufen
        if (1 == IFS0bits.T1IF && tasterGed) {
            if (0 == led) {
                decFlag = 1;
                led += decFlag;
                LATAbits.LATA11 = 1;
                LATAbits.LATA12 = 0;
            }
            else if (1 == led) {
                led += decFlag;
                LATAbits.LATA11 = 0;
                LATAbits.LATA12 = 1;
                LATAbits.LATA13 = 0;
            }
            else if (2 == led) {
                led += decFlag;
                LATAbits.LATA12 = 0;
                LATAbits.LATA13 = 1;
                LATAbits.LATA14 = 0;
            }
            else if (3 == led) {
                led += decFlag;
                LATAbits.LATA13 = 0;
                LATAbits.LATA14 = 1;
                LATAbits.LATA15 = 0;
            }
            else if (4 == led) {
                decFlag = -1;
                led += decFlag;
                LATAbits.LATA14 = 0;
                LATAbits.LATA15 = 1;
            }

            IFS0bits.T1IF = 0;
        }
        
                
    }    
}

void setAllZero() {
    LATAbits.LATA11 = 0;
    LATAbits.LATA12 = 0;
    LATAbits.LATA13 = 0;
    LATAbits.LATA14 = 0;
    LATAbits.LATA15 = 0;
}

int main(void) {
    setup();
    loop();
}