///@file
/**	Defines types that are common across nodes.
*/

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// define boolean
#define TRUE 1
#define FALSE (!TRUE)
typedef uint8_t BOOL;

// game state, can be moved to game.c/h
typedef enum {
	game_INVALID,
	game_IN_MENU,
	game_PLAYING,
	game_PAUSED,
	game_GAME_OVER,
	game_FREE_PLAY
} game_state_t;


// --- Joystick / ADC ---
/// struct for holding joystick state
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t click;
} joystick_t;

/// struct for holding slider and button state
typedef struct {
	uint8_t slider;
	uint8_t button;
} touch_t;

/// enum for holding joystick directions
typedef enum {	
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
	NEUTRAL = 4,
	//CLICKED
} direction_t;


// --- Can Bus ---


// --- CAN message SIDs ---
// \note gcc compiler flag -fshort-enums is required
enum {
	can_INVALID = 0,
	can_GAME_CMD,
	can_GAME_DATA	
};

#define game_cmd_INDEX 0
typedef enum {
    game_cmd_CHECK_BALL_DROP = 10,
	game_cmd_RESET_GAME,
	game_cmd_ACTION,
	game_cmd_SLAVE_ACK,
	game_cmd_SET_STATE,
	game_cmd_CHANGE_SETTING,
	game_cmd_CHANGE_DIFFICULTY
} game_cmd_t;

// specifiers for game_cmd
enum {
	game_cmd_setting_STANDARD = 20,
	game_cmd_setting_ALTERNATIVE,
	game_cmd_difficulty_STANDARD,
	game_cmd_difficulty_IMPOSSIBLE
};

	

// difficulty levels
typedef enum {
	game_setting_STANDARD = 20,
	game_setting_ALTERNATIVE,
} game_setting_t;


/// maximum number of bytes in a can transmission
#define CAN_MAX_BYTES 8
/// struct for holding can message
typedef struct {
	uint8_t  sid;
	uint8_t  length;
	union{
		uint8_t  data[CAN_MAX_BYTES];
		struct {
			game_cmd_t cmd_specifier;
			uint8_t cmd_data[7];
		};
	};
	
} can_msg_t;

// indices into cmd_data
#define game_joy_x_index		0
#define game_joy_y_index		1
#define game_slider_index		2
#define game_button_index		3


#endif // TYPES_H
