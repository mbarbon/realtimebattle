CPPFLAGS = $(gtkincludedirs)
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

gtkincludedirs = -I/usr/lib/glib/include -I/usr/X11R6/include
libdirs = -L/usr/lib -L/usr/X11R6/lib
libraries = -lgtk -lgdk -lglib -lXext -lX11 -lm

all: empty fire thomastest thomas2

empty: empty.o
	$(CC) $(CFLAGS) empty.o -o empty

fire: fire.o
	$(CXX) $(CFLAGS) fire.o -o fire

thomastest: thomastest.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) thomastest.o -o thomastest $(libdirs) $(libraries)

thomas2: thomas2.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) thomas2.o -o thomas2 $(libdirs) $(libraries)


empty.o: empty.c 
fire.o: fire.cc ../messagetypes.h
thomastest.o: thomastest.cc ../messagetypes.h
thomas2.o: thomas2.cc ../messagetypes.h
