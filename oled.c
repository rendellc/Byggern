#include "oled.h"
#include "fonts.h"
#include "global_declarations.h"
#include "oled_cmds.h"

#define FONTHEIGHT 6
#define FONTWIDTH 4
#define FONTCHARSX OLED_WIDTH/(FONTWIDTH+1)
#define FONTCHARSY OLED_HEIGHT/FONTHEIGHT
#define FONTMASK ((0xFF<<(8-FONTWIDTH))&0xFF)
#define SPACE 2

uint8_t* font = font4;
uint8_t* buffer = OLED_BUFFER_ADR;

uint8_t xpos = 0;
uint8_t ypos = 0;

void write_c(cmd){
	_delay_us(1);
	*((uint8_t *)OLED_C_ADR) = cmd;
}

void write_d(data){
	_delay_us(10000);
	*((uint8_t *)OLED_D_ADR) = data;
}

void oled_init(){
	// buffer test
	for (int i = 0; i<10; ++i){
		for (int j = 0; j<4; ++j){
			buffer[i*FONTWIDTH + j] = 1; // !
		}
	}
	
	uint8_t tilde[] = {0b00001000,0b00000100,0b00001000,0b00000100};
	
	write_c(SET_DISP_OFF);
	write_c(SET_SEG_REMAP);
	write_c(SET_COM_PINS_HW_CONFIG);
	write_c(0x12);
	write_c(SET_COM_SCAN_REMAP);
	write_c(SET_MULTPLX_RATIO);
	write_c(0x3f);
	write_c(SET_DISP_CLK_DIV_FOSC);
	write_c(0x80);
	write_c(SET_CONTRAST_CTRL);
	write_c(0x50);
	write_c(SET_PRE_CHRG_PERIOD);
	write_c(0x21);
	write_c(SET_MEM_ADR_MODE);
	write_c(0x00);
	write_c(SET_VCOMH_DES_LEVEL);
	write_c(0x30);
	write_c(SET_INTERNAL_IREF);
	write_c(0x00);
	write_c(SET_FOLLOW_RAM);
	write_c(SET_NORMAL_DISP);
	write_c(SET_DISP_ON);
}

void oled_reset(){
	oled_init();
	oled_fill_screen(0);
	oled_home();
	oled_update();
}

void oled_home(){
	xpos = 0;
	ypos = 0;
}

void oled_cartridgereturn(){
	xpos = 0;
}

void oled_newline(){
	if(ypos<(FONTCHARSY-1))
		ypos++;
	oled_cartridgereturn();
}

void oled_goto_line(uint8_t y){
	if(y<FONTCHARSY)
		ypos = y;
	else
		ypos = FONTCHARSY;
}

void oled_goto_column(uint8_t x){
	if(x<FONTCHARSX)
		xpos = x;
	else
		xpos = FONTCHARSX;
}

void oled_pos(uint8_t row, uint8_t column){
	oled_goto_line(row);
	oled_goto_column(column);
}

void oled_clear_line(uint8_t line){
	oled_fill_page(line, 0);
	oled_cartridgereturn();
}

void oled_print(char* msg){
	for(int i=0; msg[i]!='\0'; i++)
		oled_printchar(msg[i]);
}

void oled_printchar(uint8_t chr){
	if(chr >= 0x20)
	{
		oled_putchar(xpos, ypos, chr-32);
		oled_goto_column(++xpos);
	}
	else if (chr == '\n')
		oled_newline();
	else if (chr == '\r')
		oled_cartridgereturn();
}

//////////////////////////////////////////////////////////////////////////////////////

void oled_putchar(uint8_t x, uint8_t y, uint8_t chr){
	buffer[y*OLED_WIDTH + x] = chr - 0x20;
}

void oled_fill_screen(uint8_t val){
	write_c(SET_COL_ADR);  write_c(0x00); write_c(OLED_WIDTH-1);
	write_c(SET_PAGE_ADR); write_c(0x00); write_c(OLED_PAGES-1);
	
	for (int page = 0; page<OLED_PAGES; ++page){
		for (int seg = 0; seg<OLED_WIDTH; ++seg){
			write_d(val);
		}
	}
}

void oled_fill_line(uint8_t line, uint8_t val){

}

void oled_fill_page(uint8_t page, uint8_t val){

}

void oled_write_dot(uint8_t line, uint8_t column, uint8_t val){
}

void oled_update(){
	write_c(SET_COL_ADR); write_c(0x00); write_c(OLED_WIDTH-1);
	
	write_c(SET_PAGE_ADR);write_c(0x00); write_c(OLED_PAGES-1);
	
	write_c(SET_LOW_COL_ADR  | 0x00);
	write_c(SET_HIGH_COL_ADR | 0x00);
	
	for (int page = 0; page<OLED_PAGES; ++page){
		for (int seg = 0; seg<OLED_WIDTH; ++seg){
																_delay_ms(10); // delay for testing
																printf("hey");
			
			write_d(buffer[page*OLED_WIDTH + seg]);
		}
	}
}