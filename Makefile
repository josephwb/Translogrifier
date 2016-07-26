OBJS = main.o translog.o

CC = g++

DEBUG = -g

CFLAGS = -Wall -c -std=c++11 -O3 -funroll-loops $(DEBUG)
LFLAGS = -Wall -std=c++11 $(DEBUG)

Translogrifier: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o Translogrifier

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp
	
translog.o: translog.cpp translog.h
	$(CC) $(CFLAGS) translog.cpp
	
clean:
	rm -rf *.o Translogrifier
