///@file
/**	Defines types that are common across nodes.
*/

#ifndef TYPES_H
#define TYPES_H

// --- Joystick / ADC ---
/// struct for holding joystick state
typedef struct {
	int x;
	int y;
} joystick_t;

/// struct for holding slider and button state
typedef struct {
	int slider;
	int button;
} touch_t;

/// enum for holding joystick directions
typedef enum {	
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
	NEUTRAL = 4
} direction_t;


// --- Can Bus ---
/// maximum number of bytes in a can transmission
#define CAN_MAX_BYTES 8
/// struct for holding can message
typedef struct {
	uint8_t  sid;
	uint8_t  length;
	uint8_t  data[CAN_MAX_BYTES];
} can_msg_t;

// --- CAN message SIDs ---
#define MSG_INVALID	0 /*! invalid CAN message SID 	*/
#define MSG_JOY		1 /*! joystick CAN message SID 	*/


#endif // TYPES_H