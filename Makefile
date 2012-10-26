OBJS =Translogrifier.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -m64 -O3 -funroll-loops $(DEBUG)
LFLAGS = -Wall -m64 $(DEBUG)

Translogrifier: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o Translogrifier
	
Translogrifier.o: Translogrifier.cpp
	$(CC) $(CFLAGS) Translogrifier.cpp
	
clean:
	rm -rf *.o Translogrifier