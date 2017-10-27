/*	Defines types that are common across nodes.
*/

#ifndef TYPES_H
#define TYPES_H

// --- Joystick / ADC ---
typedef struct {
	int x;
	int y;
} joystick_t;

typedef struct {
	int slider;
	int button;
} touch_t;

typedef enum {	
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
	NEUTRAL = 4
} direction_t;


// --- Can Bus ---
#define CAN_MAX_BYTES 8
typedef struct {
	uint8_t  sid;
	uint8_t  length;
	uint8_t  data[CAN_MAX_BYTES];
} can_msg_t;

// --- CAN message types ---
#define MSG_INVALID	0
#define MSG_JOY		1


#endif // TYPES_H