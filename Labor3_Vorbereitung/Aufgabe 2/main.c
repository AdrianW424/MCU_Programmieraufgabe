#include <xc.h>
#include <stdint.h>
#include <sys/attribs.h>

typedef unsigned char u8;
typedef unsigned int u32;

void SYSTEM_Initialize(void);

int i = 0;
int freq = 0;

void setup() { 
	SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
	TRISB &= 0b011111111111111 ;     // set bit 14 of Port B to output
    
    DAC1CON = 0b11 |				// bit 0-1		: voltage reference source select bits - here it's AVdd
            0b1 << 8 |				// bit 8		: voltage reference output enable bit - output on the CVREF
            0b1 << 15 |				// bit 15		: voltage reference enable bit - enabled voltage reference
            0b11111 << 16; 			// bit 16-20 	: voltage reference selection bits - 31 * AVdd/32
    
    // für Poti
    TRISC |= 0b100000000;      	// set bit 8 of Port C to input 
    ANSELC = 0b100000000;      	// set bit 8 of Port C to ANSEL input
    
    AD1CHS = 0b00001110;       	// select input pins bits 1-3
    AD1CON1 = 0b1000;			// choose mode 12-bit ADC
    AD1CON3 = 0b1111;			// conversion clock select => 30 * TSRC = TAD
    AD1CON1SET = 0x8000;		// turn on ADC
    
    
    TMR2 = 0x0;					// clear TMR2 of Timer 2
    PR2 = 0x2EE;				// set PR2 to 750
    T2CON = 0b0010000;			// prescaler 1:2
    
    IPC4SET = 0b11000000000000000000;	// interrupt priority to 3
    IFS0CLR = 0b1000000000000000000;	// clear OC4IF
    IEC0SET = 0b1000000000000000000;	// set OC4IE
    T2CONSET = 0x8000;					// enable timer 2
}


void sinus(){
    // sinus-signal
    u8 sin[100] = {15,16,17,18,19,20,21,21,22,23,24,25,25,26,27,27,28,28,29,
                   29,29,30,30,30,30,30,30,30,30,30,29,29,29,28,28,27,27,26,25,25,24,23,22,
                   21,21,20,19,18,17,16,15,14,13,12,11,10,10,9,8,7,6,5,5,4,3,3,2,2,1,1,1,0,0,
                   0,0,0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8,9,9,10,11,12,13,14};
    
    u32 bits = 0b11 |					// bit 0-1		: voltage reference source select bits - here it's AVdd
            0b1 << 8 |					// bit 8		: voltage reference output enable bit - output on the CVREF
            0b1 << 15;					// bit 15		: voltage reference enable bit - enabled voltage reference
    
    asm volatile(
        "la $t0, %1                 \n\t"
        "li $s0, 100                \n\t"
        "add $t1, $t0, %0           \n\t"
        "beq %0, $s0, 1f            \n\t"
		"nop                        \n\t"	// vielleicht kein nop
        "lb $t0, 0($t1)             \n\t"
        "li $t1, 16                 \n\t"
        "sllv $t2, $t0, $t1         \n\t"
        "or $t2, %2, $t2            \n\t"
        "sw $t2,  DAC1CON           \n\t"
        "addi %0, %0, 1             \n\t"
        "j 2f                       \n\t"
		"nop                        \n\t"	// vielleicht kein nop
        "1:                             "
        "li %0, 0                   \n\t"
        "2:                             "
        : "+r" (i)     
        : "m" (sin), "r" (bits)   
        : "t0", "t1", "s0", "t2"
    );
    
    
    /*if(i < 100){
        DAC1CON = 0b11 |
            0b1 << 8 |
            0b1 << 15 |
            sin[i] << 16;
        i++;
    } else {
        i = 0;
    }*/
}


void poti(){
    AD1CON1SET = 0x0002;		// ADC start sampling - charge capacitor
    delay_us(1000);  			// warten
    AD1CON1CLR = 0x0002;		// ADC stop sampling - stop charging the capacitor
    
    freq = 375 + 1500 * (ADC1BUF0/4096.0);	// 2^12 = 4096
    PR2 = freq;					// set period register of timer 2 to frequency
}


__ISR(_TIMER_2_VECTOR, IPL3SOFT)TimerInterrupt(){
    sinus();
    IFS0CLR = 0b1000000000000000000;	// clear OC4IF
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

