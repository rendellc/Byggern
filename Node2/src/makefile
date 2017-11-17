CC=avr-gcc
CFLAGS=-g -Os -mmcu=atmega2560 -std=c99 -Wall
OBJS=TWI_Master.o can.o dac.o ir.o main.o mcp2515.o motor.o pwm.o spi_driver.o uart.o pi.o solonoid.o
outname=node2.out


all: $(OBJS)
	$(CC) -o $(outname) $(CFLAGS) $(OBJS)
	mv $(outname) ../../$(outname)


main.o: spi_driver.o uart.o mcp2515.o can.o pwm.o ir.o dac.o
	$(CC) $(CFLAGS) -c main.c

uart.o:
	$(CC) $(CFLAGS) -c uart.c

spi_driver.o:
	$(CC) $(CFLAGS) -c spi_driver.c

pwm.o:
	$(CC) $(CFLAGS) -c pwm.c

motor.o: dac.o uart.o pi.o
	$(CC) $(CFLAGS) -c motor.c

mcp2515.o: uart.o spi_driver.o
	$(CC) $(CFLAGS) -c mcp2515.c

ir.o:
	$(CC) $(CFLAGS) -c ir.c

dac.o: TWI_Master.o
	$(CC) $(CFLAGS) -c dac.c

can.o: mcp2515.o spi_driver.o uart.o
	$(CC) $(CFLAGS) -c can.c

TWI_Master.o:
	$(CC) $(CFLAGS) -c TWI_Master.c

solonoid.o:
	$(CC) $(CFLAGS) -c solonoid.c

pi.o:
	$(CC) $(CFLAGS) -c pi.c

.PHONY:
clean:
	rm -f *.o outname
