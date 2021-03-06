/*!@file
 * Implements a menu system.
 * All menus must have 
 * - title 
     maximum length is 23
 * - list of submenus
     maximum number is 7
 * - parent
 * - action
     callback to be excecuted when this submenu is clicked.
     If no action is wanted, then menu_action_nothing should 
     be supplied.

 */

#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>

#define MAX_TITLE_LENGTH	23 /*!< Max length of title string*/
#define MAX_SUBMENUS		7  /*!< Max number of submenus */

/// Struct for holding submenu
typedef struct _menu_t{
	char   title[MAX_TITLE_LENGTH + 1]; /*!< \todo check if using MAX_TITLE_LENGTH+1 cause problems*/
	struct _menu_t*	submenus[MAX_SUBMENUS];
	struct _menu_t* parent;
	void (*action)();				/*!< callback function when submenu is clicked */
} menu_t;

/// Empty callback function for menu
void menu_action_nothing();

/**
 * Create menu with title as a submenu of parent. 
 * If parent==NULL, then parent will be set to itself
 * @param[in] title display name of menu. Does not need '\0' termination
 * @param[in] parent pointer to parent. Will modify parent menu.
 * @param[in] action pointer to callback function
 * @return pointer to menu allocated on heap. Returns NULL if there was no room in parent.
 */
menu_t* menu_init_menu(char* title, menu_t* parent, void (*action)(void));

/**
 * Initialize menu with pre-determined menu structure.
 */
void menu_init();

/**
 * Print current menu and submenu-titles of current menu to oled display. 
 */
void menu_print_current();

/**
 * Print menu and submenu-titles of current menu to oled display. 
 */
void menu_print_menu(menu_t* menu_head);


/**
 * \deprecated
 * Read direction of joystick from ADC and update active submenu
 * \todo {deprecate this function and replace with menu_move_curor}
 */
void menu_update_subchoice();

/**
 * Move cursor for active submenus choice.
 * @param[in] {step -1 to move up, 1 to move down. 
               other values are also valid and will cause bigger step}
 */
void menu_move_cursor(int8_t step);

/**
 * Enter current submenu. Executes action before entering.
 */
void menu_enter_current(void);

/*
 * Remove all submenus of parent
 * Frees allocated memory.
 * Note, only submenus of parent are cleared.
 */
//void menu_free_submenus(menu_t* parent);

/**
 * Go up one level in the menu system.  
 */
void menu_enter_parent(void);

/**
 * Check joystick for input and update menu. 
 */
void menu_handle_input(void);


#endif /* MENU_H_ */
