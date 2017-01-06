# cphex\makefile
LIBS=
CFLAGS=-W -Wall -ansi 
all:	cli	ser	DMMPlib.o TCPLIB.o

TCPLIB.o:	TCPLIB.h	TCPLIB.c
	echo "Compilation de TCPLIB.o"
	cc -m32 -c TCPLIB.c

DMMPlib.o:	DMMPlib.h	DMMPlib.c
	echo "Compilation de DMMPlib.o"
	cc -m32 -c DMMPlib.c

cli:	cli.c	DMMPlib.o	TCPLIB.o
	echo "Compilation de client"
	cc  -m32 -o cli  cli.c DMMPlib.o	TCPLIB.o	$(LIBS)

ser:	ser.c	DMMPlib.o	TCPLIB.o
	echo "Compilation de serveur"
	cc -m32 -o ser  ser.c DMMPlib.o	TCPLIB.o	$(LIBS)

