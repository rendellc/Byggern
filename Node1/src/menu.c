/*!@file 
 * Implementation of menu system. 
 */
#include "global_declarations.h"

#include <avr/interrupt.h> /*!< \todo remove? why is this included?*/
#include <stdint.h>		
#include <string.h>	
#include <stdlib.h>
#include <util/delay.h>

#include "oled.h"
#include "menu.h"
#include "input.h"

#include "uart.h"				/*!< \todo remove, included only for debug purposes */

#include "game.h"

static menu_t* head; 			/*!< root of menu system */
static menu_t* current; 		/*!< menu currently displayed on screen */
static uint8_t subchoice = 0;	/*!< submenus currently pointed to on screen*/ 

/// implement an empty action that submenus can have.
void menu_action_nothing(){};

void menu_action_print_legal(){
	oled_fill(0x00);
	_delay_ms(10);
	//oled_home();
	/*
	fprintf(&oled_out, "Legal\n");
	fprintf(&oled_out, " TTK4115 - Byggern\n");
	fprintf(&oled_out, " Rendell Cale\n");
	fprintf(&oled_out, " William Ke\n");
	fprintf(&oled_out, " Frode van der Meeren\n");
	*/
	direction_t joy_dir;
	do {
		joystick_t joy = joy_get_state();
		joy_dir = joy_direction(joy.x,joy.y);
	} while (joy_dir != LEFT);
}

void menu_show_highscore(){
	//fprintf(&uart_out, "highscore not implemented yet\n");
	direction_t joy_dir;
	do {
		joystick_t joy = joy_get_state();
		joy_dir = joy_direction(joy.x,joy.y);
	} while (joy_dir != LEFT);
}

/*!
 * Allocate a submenu on the heap and make parent->submenu[i] point there
 * Give submenu specified title and callback.
 */
menu_t* menu_init_menu(char* title, menu_t* parent, void (*action)(void)){
	
	// test if there is more room in parent \todo implement overflow handling which works with head
	/*
	if ((parent->submenus[MAX_SUBMENUS - 1] != NULL)){
		fprintf(&uart_out, "no more room in %s\n", "a");//parent->title);
		
		return NULL;
	}
	*/
		
	menu_t* menu = (menu_t*) malloc(sizeof(menu_t));
	
	/// \note This fills in one extra '\\0' in order to guarantee proper termination
	for (uint8_t i = 0; i <= MAX_TITLE_LENGTH; i++){
		menu->title[i] = '\0';
	}
	
	/// \todo Need to guarantee terminating '\0'
	strncpy(menu->title, title, MAX_TITLE_LENGTH);	
	
	/// make sure all submenus are NULL
	for (uint8_t i = 0; i < MAX_SUBMENUS ; ++i){
		menu->submenus[i] = (menu_t*)NULL;
	}
	
	if (parent == NULL){
		menu->parent = menu;
	} else {
		menu->parent = parent;
		
		for (uint8_t i = 0; i < MAX_SUBMENUS; ++i){
			if (parent->submenus[i] == NULL){
				parent->submenus[i] = menu;
				break;
			}
		}
	}

	
	menu->action = action;
	
	return menu;
}

/*!
 * Initialize menu system 
 */
void menu_init(){
		
	head = menu_init_menu("Main", head, menu_action_nothing);
	
	menu_init_menu("Play", head, game_start);
	menu_t* menu_difficulty = menu_init_menu("Difficulty", head, menu_action_nothing);
	menu_t* menu_settings = menu_init_menu("Settings", head, menu_action_nothing);
	//menu_init_menu("Highscore", head, menu_show_highscore);
	//menu_init_menu("Legal", head, menu_action_print_legal);
	menu_init_menu("Freeplay", head, game_state_freeplay);
		
	menu_init_menu("Standard", menu_settings, game_setting_standard);
	menu_init_menu("Alternative", menu_settings, game_setting_alternative);
	
	menu_init_menu("Standard", menu_difficulty, game_difficulty_standard);
	menu_init_menu("Impossible", menu_difficulty, game_difficulty_impossible);
	
	current = head;
}


void menu_print_current(){
	oled_home();
	fprintf(&oled_out, "%s\n", current->title);
	
	for (uint8_t i = 0; i < MAX_SUBMENUS ; i++){
		if (current->submenus[i] != NULL){
			if (i == subchoice)
				fprintf(&oled_out, "->");
			else
				fprintf(&oled_out, "  ");
			
			fprintf(&oled_out, "%s\n", current->submenus[i]->title);
		}
	}
}

void menu_print_menu(menu_t* menu_head){
	menu_t* save = current;
	
	current = menu_head;
	
	menu_print_current();
	
	current = save;
}


/* Move cursor up or down by 1 step. Dont wrap around at both ends.
 * @param[in] step Move down if 1 and move up if -1. 
 */
void menu_move_cursor(int8_t step)
{
	uint8_t new_subchoice = (uint8_t)((int8_t)subchoice + step);
	
	if (current->submenus[new_subchoice] != NULL && new_subchoice < MAX_SUBMENUS){
		subchoice = new_subchoice;
	}
}


/*!
 * Clear all created menus and free memory allocated
 * \todo {implement this}
 */
void menu_exit(menu_t* head)
{
	menu_t* curr = head;
	
	while (curr->submenus[0] != NULL) // go to deepest level in tree
		curr = curr->submenus[0];
	
	curr = curr->parent;

}


/// remove all submenus of parent from heap
void menu_free_submenus(menu_t* parent)
{
	for (uint8_t i = 0; i<MAX_SUBMENUS; ++i)
	{
		///\todo this can't be right?
		// should probably check for parent->submenu[i] != NULL
		if (1)
			free(&parent->submenus[i]);
	}
}



void menu_enter_current(void)
{
	oled_fill(0x00); // needed if next submenu has fewer menus than current
	
	// check if current choice is valid and that current choice has any children
	if (current->submenus[subchoice] != NULL && current->submenus[subchoice]->submenus[0] != NULL){
		//fprintf(&uart_out, "current: %p\n", current);
		//fprintf(&uart_out, "next:    %p\n", current->submenus[subchoice]);

		current = current->submenus[subchoice];
		subchoice = 0;
	}
	
	current->submenus[subchoice]->action();
}


void menu_enter_parent(void){
	oled_fill(0x00);
	
	current = current->parent;	
}


/**
 * Check for joystick input and update menu choices.
 */
void menu_handle_input(void){                
        
    static BOOL joy_released = TRUE;
	
	joystick_t joy = joy_get_state();
	
	direction_t joy_dir = joy_direction(joy.x, joy.y);
	
	switch (joy_dir){
		case UP:
			if (joy_released){
				joy_released = FALSE;
				menu_move_cursor(-1);
			}
			break;
		case DOWN:
			if (joy_released){
				joy_released = FALSE;
				menu_move_cursor(1);
			}
			break;
			
		case RIGHT:
			if (joy_released){
				joy_released = FALSE;
				menu_enter_current();
			}
			break;
		
		case LEFT:
			if (joy_released){
				joy_released = FALSE;
				menu_enter_parent();
			}
			break;
		
		case NEUTRAL:
			joy_released = TRUE;
			break;
	}
	
}

