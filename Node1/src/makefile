CC=avr-gcc
CFLAGS=-g -Os -mmcu=atmega162 -std=c99 -Wall
OBJS=uart.o adc.o oled.o spi_driver.o can.o joystick.o main.o mcp2515.o menu.o
outname=node1.out


all: $(OBJS)
	$(CC) -o $(outname) $(CFLAGS) $(OBJS)

main.o: uart.o adc.o oled.o menu.o spi_driver.o can.o joystick.o mcp2515.o
	$(CC) $(CFLAGS) -c main.c

uart.o:
	$(CC) $(CFLAGS) -c uart.c

adc.o:
	$(CC) $(CFLAGS) -c adc.c	

can.o: mcp2515.o spi_driver.o uart.o
	$(CC) $(CFLAGS) -c can.c

mcp2515.o: uart.o spi_driver.o
	$(CC) $(CFLAGS) -c mcp2515.c

joystick.o: can.o adc.o uart.o
	$(CC) $(CFLAGS) -c joystick.c

menu.o: oled.o joystick.o uart.o
	$(CC) $(CFLAGS) -c menu.c

oled.o:
	$(CC) $(CFLAGS) -c oled.c

spi_driver.o:
	$(CC) $(CFLAGS) -c spi_driver.c

sram_test.o:
	$(CC) $(CFLAGS) -c sram_test.c	


.PHONY:
clean:
	rm -f *.o outname

