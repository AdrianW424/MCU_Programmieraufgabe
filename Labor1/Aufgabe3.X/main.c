/*
	Toggle LED with Button
*/

#include <stdint.h>
#include <xc.h>

void setup()
{    
    //SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    TRISDbits.TRISD4 = 0;  //Set D4 for output
    PR1 = 18661;
    T1CONbits.TCKPS0 = 1;   // set prescaler to 256
    T1CONbits.TCKPS1 = 1;
    T1CONbits.TCS = 0;      // PCBLK input (the default)
}

void loop() {
    //Turn off LED
    PORTDbits.RD4 = 0;
    
    /* maybe: set RB9 as Input */
    int flag = 1;
    
    while (1) {  
        
        if (0 == PORTBbits.RB9 && flag) {
            flag = 0;
            // LED turn
            T1CONbits.ON = !T1CONbits.ON;  
            
        } else if (1 == PORTBbits.RB9) {
            flag = 1;
        }
        
        // Ist Timer abgelaufen?
        if (1 == IFS0bits.T1IF) {
            LATDbits.LATD4 = !LATDbits.LATD4;
            IFS0bits.T1IF = 0;
        }
        
    }    
}
    


int main(void) {
    setup();
    loop();
}