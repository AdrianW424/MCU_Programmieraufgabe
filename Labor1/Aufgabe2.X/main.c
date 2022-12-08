/*
	Toggle LED with Button
*/

#include <stdint.h>
#include <xc.h>

void setup()
{    
    //SYSTEM_Initialize();  // set 24 MHz clock for CPU and Peripheral Bus
                          // clock period = 41,667 ns = 0,0417 us
    TRISDbits.TRISD4 = 0;                          //Set D4 for output
    
}

void loop() {
    //Turn off LED
    PORTDbits.RD4 = 0;
    
    /* maybe: set RB9 as Input */
    int flag = 1;
    
    while (1) { 
        // Check if Button is pressed
        if (0 == PORTBbits.RB9 && flag) {
            flag = 0;
            //Yes, it is pressed
            // LED turn
            if (0 == LATDbits.LATD4) {
                LATDbits.LATD4 = 1;
            }
            else {
                LATDbits.LATD4 = 0;
            }
            
        } else if (1 == PORTBbits.RB9) {
            flag = 1;
        }
        
    }    
}
    


int main(void) {
    setup();
    loop();
}