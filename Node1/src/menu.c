/*!@file 
 * Implementation of menu system. 
 */
#include <avr/interrupt.h> /*!< \todo remove? why is this included?*/
#include <stdint.h>		
#include <string.h>	
#include <stdlib.h>	

#include "oled.h"
#include "menu.h"
#include "joystick.h"
#include "global_declarations.h"
#include "uart.h"				/*!< \todo remove, included only for debug purposes */

#include "game.h"

static menu_t* head; 			/*!< root of menu system */
static menu_t* current; 		/*!< menu currently displayed on screen */
static uint8_t subchoice = 0;	/*!< submenus currently pointed to on screen*/ 

/// implement an empty action that submenus can have.
void menu_action_nothing(){
	fprintf(&uart_out, "nothing\n");
};

/// Setup atmega162 IO to interface with clicker on joystick.
void menu_click_init(){
	joystick_init();
	//PORTE  |= 1 << PE0;
	//DDRE   &= ~(1 << PE0);
}

/*!
 * Allocate a submenu on the heap and make parent->submenu[i] point there
 * Give submenu specified title and callback.
 */
menu_t* menu_init_menu(char* title, menu_t* parent, void (*action)(void)){
	
	// test if there is more room in parent
	if (parent != head && parent->submenus[MAX_SUBMENUS - 1] != NULL){
		fprintf(&uart_out, "no more room in %s\n", parent->title);
		
		return NULL;
	}
		
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
	
	///\todo {must also be added when the else{...} statement excecutes}
	menu->action = action;
	
	return menu;
}

/*!
 * Initialize menu system 
 */
void menu_init(){
	menu_click_init();
		
	head = menu_init_menu("Main", head, menu_action_nothing);
	
	menu_init_menu("Start", head, game_start);
	menu_init_menu("Difficulty", head, menu_action_nothing);
	menu_t* menu_highscore = menu_init_menu("Highscore", head, menu_action_nothing);
	menu_t* menu_legal = menu_init_menu("Legal", head, menu_action_nothing);
	
	menu_init_menu("TTK4115 - Byggern", menu_legal, menu_action_nothing);
	menu_init_menu("Rendell Cale", menu_legal, menu_action_nothing);
	menu_init_menu("William Ke", menu_legal, menu_action_nothing);
	menu_init_menu("Frode van der Meeren", menu_legal, menu_action_nothing);
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

/// \todo {deprecate this}
/// \deprecated
/*
void menu_update_subchoice()
{	
	direction_t adc_joy = adc_direction_joy();
	
	switch (adc_joy)
	{
		case UP:
			if (subchoice > 0)
				--subchoice;
			break;
		case DOWN:
			if ((subchoice+1) < MAX_SUBMENUS && current->submenus[subchoice+1])
				++subchoice;
			break;
		case LEFT:
			current = current->parent;
			subchoice = 0;
			break;
		case RIGHT:
			if(current->submenus[subchoice]->submenus[0] != NULL)
			{
				// fprintf(&uart_out, "%s er valgt!\n", current->submenus[subchoice]->title);
				// TODO: Nå vet vi at en sluttfunksjon er valgt, gjør handlingen!
				current->action();
			}
			else
			{
				current = current->submenus[subchoice];
				subchoice = 0;
			}
			break;
		default:
			break;		
	}
	
	if (adc_joy==RIGHT || (adc_joy==LEFT && current != head))
		oled_reset();
	
	menu_print_current();
}
*/


/* Move cursor up or down by 1 step. Dont wrap around at both ends.
 * @param[in] step Move down if 1 and move up if -1. 
 */
void menu_move_cursor(int8_t step)
{
	uint8_t new_subchoice = (uint8_t)((int8_t)subchoice + step);
	
	fprintf(&uart_out, "new_subchoice: %u\n", new_subchoice);
	
	if (current->submenus[new_subchoice] != NULL && new_subchoice < MAX_SUBMENUS){
		subchoice = new_subchoice;
	}
	/*
	else if (new_subchoice == (uint8_t)-1){
		oled_fill(0x00);
		current = current->parent;	
	}
	*/
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
	
		fprintf(&uart_out, "nothing: \t%p\n", menu_action_nothing);
		fprintf(&uart_out, "action : \t\t%p\n", (*(current->submenus[subchoice]->action)));
		
		current = current->submenus[subchoice];
		subchoice = 0;
	}
	
	current->submenus[subchoice]->action();
}


void menu_enter_parent(void){
	oled_fill(0x00);
	
	current = current->parent;
	
}