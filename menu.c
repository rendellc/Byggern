/*
 * menu.c
 *
 * Created: 29.09.2017 09:36:40
 *  Author: rendellc
 */ 

#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>

#include "oled.h"
#include "menu.h"
#include "adc.h"
#include "uart.h"






menu_t* head;
menu_t* current;
uint8_t subchoice = 0;


void menu_click_init(){
	//cli();
	PORTE  |= 1 << PE0;
	DDRE   &= ~(1 << PE0);
	//sei();
}

menu_t* menu_init_menu(char* title, menu_t* parent){
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
	return menu;
}

void menu_init(){
	//menu_click_init();
	
	head = menu_init_menu("Mainaaaaaaaaaaaaaaaaaaa", NULL);
	menu_t* under2 = menu_init_menu("Under 2", head);
	menu_init_menu("Under under", under2);
	menu_init_menu("Under 1", head);
	menu_init_menu("Under a", head);
	menu_init_menu("Under b", head);
	menu_init_menu("Under c", head);
	menu_init_menu("Under d", head);
	menu_init_menu("Under e", head);
		
	current = head;
	
	menu_print_current();
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

void menu_update_subchoice(){
	
	if (adc_direction_joy() == UP && subchoice > 0)
	{
		subchoice--;
	}
	else if(adc_direction_joy() == DOWN && (subchoice+1) < MAX_SUBMENUS && current->submenus[subchoice+1] != NULL)
	{
		subchoice++;
	}
	else if (adc_direction_joy() == LEFT)
	{
		current = current->parent;
		subchoice = 0;
		oled_reset();
	}
	else if (adc_direction_joy() == RIGHT)
	{
		if(current->submenus[subchoice]->submenus[0] == NULL)
		{
			fprintf(&uart_out, "%s er valgt!\n", current->submenus[subchoice]->title);
			// TODO: Nå vet vi at en sluttfunksjon er valgt, gjør handlingen!
		}
		else
		{
			current = current->submenus[subchoice];
			subchoice = 0;
			oled_reset();
		}
	}
	
	menu_print_current();
}
