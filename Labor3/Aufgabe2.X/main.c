#include <xc.h>
#include <stdint.h>
#include <sys/attribs.h>
#include "sinus.h"

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);

int i = 0;
int freq = 0;

// etwa 5 nF Kodensator

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
	TRISBbits.TRISB14 = 0;     // set bit 14 of Port B to output
    
    DAC1CON = 0b11 |			// bit 0-1		: voltage reference source select bits - here it's AVdd
            0b1 << 8 |			// bit 8		: voltage reference output enable bit - output on the CVREF
            0b1 << 15 |			// bit 15		: voltage reference enable bit - enabled voltage reference
            0b11111 << 16; 		// bit 16-20 	: voltage reference selection bits - 31 * AVdd/32
    
    
	// für Poti
    TRISCbits.TRISC8 = 1;      	// set bit 8 of Port C to input
    ANSELC = 0b100000000;      	// set bit 8 of Port C to ANSEL input
    
    AD1CHS = 0b00001110;       	// select input pins bits 1-3
    AD1CON1 = 0b1000;			// choose mode 12-bit ADC
    AD1CON3 = 0b1111;			// conversion clock select => 30 * TSRC = TAD
    AD1CON1SET = 0x8000;		// turn on ADC
    
    
    TMR2 = 0x0;					// clear TMR2 of Timer 2
    PR2 = 0x2EE;				// set PR2 to 750
    T2CON = 0b0010000;			// prescaler 1:2
	// => mit Prescaler 1:2 wird f zu 12 MHz, für 500 Hz brauchen wir 12 MHz / (500*32) = 750 für das Periodenregister
    
    IPC4SET = 0b11000000000000000000;	// interrupt priority to 3
    IFS0CLR = 0b1000000000000000000;	// clear IF
    IEC0SET = 0b1000000000000000000;	// set IE
    T2CONSET = 0x8000;					// enable timer 2
	//__builtin_enable_interrupts();
}


void nextOutput(){
    /*
    DAC1CONbits.DACDAT = sinus[i];
    i++;
    if(i >= 100){
        i = 0;
    }
    */
    
    asm volatile(
    "la $t0, sinus              \n\t"
    "add $t1, %[i], $t0         \n\t"
    "lb $t2, 0($t1)             \n\t"
    "add %[dac], $t2, $zero     \n\t"
    "addi %[i], %[i], 1         \n\t"
    "addi $t3, $zero, 100       \n\t"
    "bne %[i], $t3, 1f          \n\t"
    "nop                        \n\t"
    "add %[i], $zero, $zero     \n\t"
    "1:                             "
    :[i]"+r"(i), [dac]"=r"(DAC1CONbits.DACDAT)
    :
    :"t0", "t1", "t2", "t3"
    );
}


void poti(){
    AD1CON1SET = 0x0002;		// ADC start sampling - charge capacitor
    delay_us(500);				// warten
    AD1CON1CLR = 0x0002;		// ADC stop sampling - stop charging the capacitor
    while(0 == AD1CON1bits.DONE);
    
    freq = 121 + ((479*ADC1BUF0)/4095);	// 2^12 = 4096 -> ohne Gleitkomma
    // wenn Poti max. links, dann ADC1BUF0 = 4095
    // wenn Poti max. rechts, dann ADC1BUF0 = 0
    PR2 = freq;					// set period register of timer 2 to frequency
}


void __ISR(_TIMER_2_VECTOR, IPL3SOFT)TimerInterrupt(){
    nextOutput();
    IFS0CLR = 0b1000000000000000000;	// clear IF
}

void loop() { 
    while(1){
        poti();
    }
}

int main(void) {
    setup();
    loop();
}
