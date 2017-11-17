/*!@file
* implementaion of functions for solenoid
*/

#include <avr/io.h>
#include <util/delay.h>
#include "solenoid.h"


void solenoid_init(){
    DDRE |= (1 << PE4); /**< set PE¤ to output port */ 
}


void solenoid_trigger(){
    PORTE &= ~(1 << PE4);

    // \todo Use interrupt to disable solenoid
    _delay_us(10); // find good timing? 

    PORTE |= (1 << PE4);
}