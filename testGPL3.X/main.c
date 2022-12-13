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

void initButton();

void setup() {   
    initButton();
    
    __builtin_enable_interrupts();
}

void initButton() {
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

int menu = 0;

void __ISR(_EXTERNAL_1_VECTOR, IPL6SOFT) Button1ISR(void) {
    
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
}

int main(void) {
    setup();
    while(1) {
        
    }
}