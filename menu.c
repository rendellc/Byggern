/*
 * menu.c
 *
 * Created: 29.09.2017 09:36:40
 *  Author: rendellc
 */ 

#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "oled.h"
#include "menu.h"
#include "adc.h"
#include "uart.h"

menu_t* head;
menu_t* current;
uint8_t subchoice = 0;

void menu_action_nothing(){};


void menu_click_init(){
	//cli();
	PORTE  |= 1 << PE0;
	DDRE   &= ~(1 << PE0);
	//sei();
}

menu_t* menu_init_menu(char* title, menu_t* parent, void (*action)(void)){
	menu_t* menu = (menu_t*) malloc(sizeof(menu_t));	
	
	for (uint8_t i = 0; i <= MAX_TITLE_LENGTH ; i++){
		menu->title[i] = '\0';
	}
	strncpy(menu->title, title, MAX_TITLE_LENGTH);	// Huske å ha plass til null i slutten
	
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
		return NULL; // return NULL if no more room
	}
	
	menu->action = action;
	
	return menu;
}

void menu_init(){
	//menu_click_init();
	
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

/// Delete all menus from heap
void menu_exit(menu_t* head)
{
	menu_t* curr = head;
	
	while (curr->submenus[0] != NULL) // go to deepest level in tree
		curr = curr->submenus[0];
	
	curr = curr->parent;

}

void menu_free_submenus(menu_t* parent)
{
	for (uint8_t i = 0; i<MAX_SUBMENUS; ++i)
	{
		if (1)
			free(&parent->submenus[i]);
	}
}