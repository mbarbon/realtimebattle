CC = gcc
CXX = g++

ifndef DEBUG_MODE
DEBUG_MODE=no
endif

ifeq ($(DEBUG_MODE), yes)
#debug-options
CPPFLAGS = $(gtkincludedirs)
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall
else
#final-options
CPPFLAGS = $(gtkincludedirs)
CFLAGS = -O
CXXFLAGS = -O
endif

gtkincludedirs = `gtk-config --cflags`
libs = `gtk-config --libs`

target: empty fire thomastest thomas2

empty: empty.o
	$(CC) $(CFLAGS) empty.o -o empty

fire: fire.o
	$(CXX) $(CFLAGS) fire.o -o fire

thomastest: thomastest.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) thomastest.o -o thomastest $(libs)

thomas2: thomas2.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) thomas2.o -o thomas2 $(libs)


empty.o: empty.c 
fire.o: fire.cc ../src/Messagetypes.h
thomastest.o: thomastest.cc ../src/Messagetypes.h
thomas2.o: thomas2.cc ../src/Messagetypes.h

clean:
	rm -f *~ *.o core*
