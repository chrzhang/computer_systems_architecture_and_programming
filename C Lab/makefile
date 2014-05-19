#
# Makefile that builds the executable dic for the C Lab
#

CC = gcc
CFLAGS = -O -Wall -m32 -std=c99 -pedantic -Wextra
LIBS = -lm
EXECUTABLE = lab2
EXTRACREDIT = lab2-bonus

all: $(EXECUTABLE) $(EXTRACREDIT)

$(EXECUTABLE): lab2.c
	$(CC) $(CFLAGS) $(LIBS) -o $(EXECUTABLE) lab2.c

$(EXTRACREDIT): lab2-bonus.c
	$(CC) $(CFLAGS) $(LIBS) -o $(EXTRACREDIT) lab2-bonus.c

clean:
	rm -f *.o $(EXECUTABLE) $(EXTRACREDIT) *~
