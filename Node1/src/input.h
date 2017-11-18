/**@file
 * Joystick interface for node 1.
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "global_declarations.h"

/// Convert range from 0-to-255 to -100-to-100
int8_t convert_range(uint8_t data);

/**
 * Calculate direction of joystick based on coordinates
 * @param[in] joy_x X axis of joystick
 * @param[in] joy_y Y axis of joystick
 */
direction_t joy_direction(uint8_t joy_x, uint8_t joy_y);

/// Send slider position on can bus. Used for testing. 
void send_slider(void);

/// Initialize joystick
void joystick_init(void);

/// get current state of joystick
joystick_t joy_get_state();

/*!
 * Read joystick and send it as a message on the can bus.
 * Always sends on can channel 0. 
 */
void send_joy(void);

/*!
 * Read the right slider and right button and return result as a touch_t.
 */
touch_t touch_get_state();


#endif /* JOYSTICK_H_ */