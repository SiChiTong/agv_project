#
# FILE:
# Makefile
#
# FUNCTION:
# Makefile for modbus 

CC=gcc

NAME=libmodbus
VERSION=0.0.4

CFLAGS	= -Wall -O3 -fPIC

all: libmodbus

libmodbus: commun.o  mbs.o mbm.o mbdialer.o
	$(CC) $(CFLAGS) -o libmodbus.so commun.o mbs.o  mbm.o mbdialer.o -lpthread  -shared -I.


commun.o: commun.c modbus.h
	$(CC) $(CFLAGS) -c commun.c -I.


mbs.o: mbs.c modbus.h 
	$(CC) $(CFLAGS) -c  mbs.c -I.

mbm.o: mbm.c modbus.h 
	$(CC) $(CFLAGS) -c  mbm.c -I.

mbdialer.o: mbdialer.c modbus.h
	$(CC) $(CFLAGS) -c  mbdialer.c -I.

clean:
	rm -f  ~* *~ *.ASV *.bak *.o core> /dev/null

install:
	chown root.root libmodbus.so
	cp libmodbus.so /usr/lib/
	cp modbus.h /usr/include
	
uninstall:
	rm /usr/lib/libmodbus.so
	rm /usr/include/modbus.h
	rm /usr/doc/libmodbus -rf

tar:	clean
	tar -C .. -c -v -z -f ../tar/$(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
