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

// game state
typedef enum {
	game_INVALID,
	game_IN_MENU,
	game_PLAYING,
	game_PAUSED
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
/// maximum number of bytes in a can transmission
#define CAN_MAX_BYTES 8
/// struct for holding can message
typedef struct {
	uint8_t  sid;
	uint8_t  length;
	uint8_t  data[CAN_MAX_BYTES];
} can_msg_t;

// --- CAN message SIDs ---
// \note gcc compiler flag -fshort-enums is required
enum {
	can_INVALID = 0,
	can_GAME_CMD,
	can_GAME_INFO,
	can_GAME_DATA	
};


//define MSG_INVALID		0 /*!< invalid CAN message SID */
//#define MSG_JOY			1 /*!< joystick CAN message SID */
//#define MSG_SLIDER		2 /*!< slider CAN message SID */
//#define MSG_GAME_CMD		3 /*!< Game command CAN message SID */
//#define MSG_GAME_INFO		4 /*!< Command ack CAN message SID */
//#define MSG_GAME_DATA		5 /*!< Game data CAN message SID */

#define game_cmd_INDEX 0
enum {
    game_cmd_CHECK_BALL_DROP = 10,
	game_cmd_RESET_GAME,
	game_cmd_ACTION,
	game_cmd_SLAVE_ACK,
	game_cmd_SET_STATE,
	game_cmd_CHANGE_SETTING,
};

// settings for game

typedef enum {
	game_setting_STANDARD = 20,
	game_setting_ALTERNATIVE
} game_setting_t;

#define game_setting_standard_turn	1
#define game_setting_standard_fire	2
#define game_setting_standard_motor	3

#define game_setting_alternative_turn	1
#define game_setting_alternative_fire	2
#define game_setting_alternative_motor	3


#endif // TYPES_H
