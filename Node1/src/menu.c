/*!@file 
 * Implementation of menu system. 
 */
#include <avr/interrupt.h> /*!< \todo remove? why is this included?*/
#include <stdint.h>		
#include <string.h>	
#include <stdlib.h>	

#include "oled.h"
#include "menu.h"
#include "joystick.h" /*!< \tode remove */
#include "global_declarations.h"
#include "uart.h" /*!< \todo remove, included only for debug purposes */

static menu_t* head; 			/*!< root of menu system */
static menu_t* current; 		/*!< menu currently displayed on screen */
static uint8_t subchoice = 0;  /*!< submenus currently pointed to on screen*/ 

/// implement an empty action that submenus can have.
void menu_action_nothing(){};

/// Setup atmega162 IO to interface with clicker on joystick.
void menu_click_init(){
	PORTE  |= 1 << PE0;
	DDRE   &= ~(1 << PE0);
}

/*!
 * Allocate a submenu on the heap and make parent->submenu[i] point there
 * Give submenu specified title and callback.
 */
menu_t* menu_init_menu(char* title, menu_t* parent, void (*action)(void)){
	menu_t* menu = (menu_t*) malloc(sizeof(menu_t));	
	
	for (uint8_t i = 0; i <= MAX_TITLE_LENGTH ; i++){
		menu->title[i] = '\0';
	}
	
	strncpy(menu->title, title, MAX_TITLE_LENGTH);	// Need to guarantee terminating '\0'
	
	for (uint8_t i = 0; i < MAX_SUBMENUS ; i++){
		menu->submenus[i] = (menu_t*)NULL;
	}
	
	if (parent == NULL){
		menu->parent = menu;
	} else {
		menu->parent = parent;
		
		for (int i=0; i < MAX_SUBMENUS; i++){
			if (parent->submenus[i] == NULL){
				parent->submenus[i] = menu;
				return menu;
			}
		}

		/** \todo 
			this is wrong. Should only return NULL when there is no more
			room in parent menu. Should also free heap allocated memory in this case.
		*/
		return NULL; // return NULL if no more room
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
	
	head = menu_init_menu("Mainaaa", NULL, NULL);
	
	menu_init_menu("Under 1", head, menu_action_nothing);
	menu_init_menu("Under 2", head, menu_action_nothing);

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


/// Move cursos specified step. Wrap around at both ends.
void menu_move_cursor(int8_t step){
	///\test this has not been tested
	subchoice = (uint8_t)(((int8_t)subchoice + step)%MAX_SUBMENUS);
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