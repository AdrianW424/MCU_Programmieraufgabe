#include <xc.h>
#include <stdint.h>
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
    //__builtin_enable_interrupts();
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

void __ISR(_TIMER_1_VECTOR, IPL2SOFT) Timer1ISR(){
    if (count == 0) {
        LATAbits.LATA9 = 0;
        count++;
    } else if (count == 9) {
        LATAbits.LATA9 = 1;
        count = 0;
    } else {
        count++;
    }
    
    TMR1 = 0;               // reset timer
    // Reset Timer
    IFS0bits.T1IF = 0;      // clear interrupt-flag
}

u32 readSensor() {
    
}

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    initI2C();
    initLCD();
    initTimer();
}

void loop() {
    /*
    while(1) {
        int i = 12;
        for(int i2 = 0; i2 <= i; i2++) {
            clearLCD();
            writeLCD("Hallo Jannik", i2);
            delay_us(1000000);
        }
    }
    */
    /*
    TRISAbits.TRISA9 = 0;  // RA9 to output
    while(1) {
        if (count == 0) {
            LATAbits.LATA9 = 0;
            count++;
        } else if (count == 9) {
            LATAbits.LATA9 = 1;
            count = 0;
        } else {
            count++;
        }

        delay_us(9);    // delay_us(10) nicht genau 10us, wegen Verzögerung --> deswegen dealy_us(9) gewählt
    }
    */
}

int main(void) {
    setup();
    loop();
}
