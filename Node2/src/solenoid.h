



#include <avr/io.h>
#include <util/delay.h>


void solenoid_init(){
    DDRE |= (1 << PE4);
}


void solenoid_trigger(){
    PORTE &= ~(1 << PE4);

    // \todo Use interrupt to disable solenoid
    _delay_us(10); // find good timing? 

    PORTE |= (1 << PE4);
}