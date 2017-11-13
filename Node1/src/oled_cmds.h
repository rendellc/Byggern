/* @file    
 * Commands for OLED as per table 8-1 of 
 * "OLED SSD1308 (controller).pdf"
 */

#ifndef OLED_CMDS_H_
#define OLED_CMDS_H_

#define SET_LOW_COL_ADR		0x00
#define SET_HIGH_COL_ADR	0x10

#define SET_SEG_NO_REMAP	0xa0
#define SET_SEG_REMAP		0xa1
#define SET_FOLLOW_RAM		0xa4
#define SET_IGNORE_RAM		0xa5
#define SET_NORMAL_DISP		0xa6
#define	SET_INVERS_DISP		0xa7
#define SET_MULTPLX_RATIO	0xa8
#define SET_INTERNAL_IREF	0xad
#define SET_DISP_OFF		0xae
#define SET_DISP_ON			0xaf

#define SET_PAGE_START		0xb0

#define SET_COM_SCAN_NORMAL 0xc0
#define SET_COM_SCAN_REMAP	0xc8

#define SET_MEM_ADR_MODE	0x20
#define SET_COL_ADR			0x21
#define	SET_PAGE_ADR		0x22

#define SET_CONTRAST_CTRL	0x81



#define SET_DISP_OFFSET			0xd3
#define SET_DISP_CLK_DIV_FOSC	0xd5
#define SET_PRE_CHRG_PERIOD		0xd9
#define SET_COM_PINS_HW_CONFIG	0xda
#define SET_VCOMH_DES_LEVEL		0xdb

#define OLED_NOP				0xe3 

#endif /* OLED_CMDS_H_ */