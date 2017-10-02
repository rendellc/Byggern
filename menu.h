/*
 * menu.h
 *
 * Created: 29.09.2017 10:18:52
 *  Author: rendellc
 */ 


#ifndef MENU_H_
#define MENU_H_

#define MAX_TITLE_LENGTH	23
#define MAX_SUBMENUS		7

typedef struct _menu_t {
	char   title[MAX_TITLE_LENGTH+1];
	union{	
		struct _menu_t*	submenus[MAX_SUBMENUS];
		void (*callback)(void);
	}
	struct _menu_t* parent;
} menu_t;

menu_t* menu_init_menu(char*, menu_t*);
void menu_init();
void menu_print_current();
void menu_down();
void menu_up();
void menu_update_subchoice();


#endif /* MENU_H_ */
