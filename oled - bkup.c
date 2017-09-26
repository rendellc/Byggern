#include "oled.h"
#include "fonts.h"
#include "global_declarations.h"
#include "oled_cmds.h"

#define FONTHEIGHT 6
#define FONTWIDTH 4
#define FONTCHARSX OLED_WIDTH/(FONTWIDTH+1)
#define FONTCHARSY OLED_HEIGHT/FONTHEIGHT
#define FONTMASK 0xFF<<(8-FONTWIDTH)
uint8_t* font = font4;

uint8_t xpos = 0;
uint8_t ypos = 0;

void write_c(cmd){
	_delay_us(1);
	char* ptr = OLED_C_ADR;
	*ptr = cmd;
}

void write_d(data){
	_delay_us(1);
	char* ptr = OLED_D_ADR;
	*ptr = data;
}

void oled_init(){
	write_c(SET_DISP_OFF);        // display  off
	write_c(SET_SEG_REMAP);        // segment  remap
	write_c(SET_COM_PINS_HW_CONFIG);					// common  pads  hardware:  alternative
	write_c(0x12);
	write_c(SET_COM_SCAN_REMAP);        // common output scan direction:com63~com0
	write_c(SET_MULTPLX_RATIO);        // multiplex  ration  mode:63
	write_c(0x3f);
	write_c(SET_DISP_CLK_DIV_FOSC);        // display divide ratio/osc. freq. mode
	write_c(0x80);
	write_c(SET_CONTRAST_CTRL);        // contrast  control
	write_c(0x50);
	write_c(SET_PRE_CHRG_PERIOD);        // set  pre-charge  period
	write_c(0x21);
	write_c(SET_MEM_ADR_MODE);        // Set  Memory  Addressing  Mode
	write_c(0x00);
	write_c(SET_VCOMH_DES_LEVEL);        // VCOM  deselect  level  mode
	write_c(0x30);
	write_c(SET_INTERNAL_IREF);        // master  configuration
	write_c(0x00);
	write_c(SET_FOLLOW_RAM);			// out  follows  RAM  content
	write_c(SET_NORMAL_DISP);			 // set  normal  display
	write_c(SET_DISP_ON);				// display  on	
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

void oled_carriagereturn(){
	xpos = 0;
}

void oled_newline(){
	if(ypos<(FONTCHARSY-1))
		ypos++;
	oled_carriagereturn();
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
		oled_carriagereturn();
}

//////////////////////////////////////////////////////////////////////////////////////

void oled_putchar(uint8_t x, uint8_t y, uint8_t chr){
	char* ptr = OLED_BUFFER_ADR;
	for(int i=0; i<FONTWIDTH; i++)
	{
		ptr[y*FONTHEIGHT+x*(FONTWIDTH+1)+i] &= ~(0xFF&FONTMASK);
		ptr[y*FONTHEIGHT+x*(FONTWIDTH+1)+i] |= font[chr*FONTWIDTH + i];
	}
	ptr[y*FONTHEIGHT+x*(FONTWIDTH+1)+FONTWIDTH] &= ~(0xFF&FONTMASK);
}

void oled_fill_screen(uint8_t val)
{
	for(int i = 0; i<OLED_HEIGHT; i++)
		oled_fill_line(i,val);
}

void oled_fill_line(uint8_t line, uint8_t val){
	for (int i = 0; i < OLED_WIDTH; i++)
		oled_write_dot(line, i, val);
}

void oled_fill_page(uint8_t page, uint8_t val){
	char* ptr = OLED_BUFFER_ADR;
	for (int i = 0; i < OLED_WIDTH; i++)
		ptr[i+page*OLED_WIDTH] = val;
}

void oled_write_dot(uint8_t line, uint8_t column, uint8_t val){
	char* ptr = OLED_BUFFER_ADR;
	if(val)
		ptr[column+(line>>3)*OLED_WIDTH] |= 1<<(line&0x07);
	else
		ptr[column+(line>>3)*OLED_WIDTH] &= ~(1<<(line&0x07));
}

void oled_update(){
	write_c(0x21);
	write_c(0x00);
	write_c(OLED_WIDTH-1);
	write_c(0x22);
	write_c(0x00);
	write_c(OLED_HEIGHT-1);
	char* ptr = OLED_BUFFER_ADR;
	for(int i=0; i<OLED_WIDTH*OLED_PAGES; i++)
	write_d(ptr[i]);
}