CC=avr-gcc
CFLAGS=-g -Os -mmcu=atmega162 -std=c99 -Wall
OBJS=uart.o adc.o oled.o spi_driver.o can.o input.o main.o mcp2515.o menu.o game.o sram_test.o lyd.o
outname=node1.out


all: $(OBJS)
	$(CC) -o $(outname) $(CFLAGS) $(OBJS)
	mv $(outname) ../../$(outname)

main.o: uart.o adc.o oled.o menu.o spi_driver.o can.o input.o mcp2515.o
	$(CC) $(CFLAGS) -c main.c

uart.o:
	$(CC) $(CFLAGS) -c uart.c

adc.o:
	$(CC) $(CFLAGS) -c adc.c	

can.o: mcp2515.o spi_driver.o uart.o
	$(CC) $(CFLAGS) -c can.c

mcp2515.o: uart.o spi_driver.o
	$(CC) $(CFLAGS) -c mcp2515.c

input.o: can.o adc.o uart.o
	$(CC) $(CFLAGS) -c input.c

menu.o: oled.o input.o uart.o
	$(CC) $(CFLAGS) -c menu.c

oled.o:
	$(CC) $(CFLAGS) -c oled.c

spi_driver.o:
	$(CC) $(CFLAGS) -c spi_driver.c

sram_test.o:
	$(CC) $(CFLAGS) -c sram_test.c	

game.o: lyd.o oled.o uart.o menu.o can.o input.o
	$(CC) $(CFLAGS) -c game.c


.PHONY:
clean:
	rm -f *.o outname
