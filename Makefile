ROBOTDIR = Robots

CC = gcc
CXX = g++

#debug-options
CPPFLAGS = $(gtkincludedirs)
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

#normal-options
#CPPFLAGS = $(gtkincludedirs)
#CFLAGS = -Wall
#CXXFLAGS = -Wall

#final-options
#CPPFLAGS = $(gtkincludedirs)
#CFLAGS = -O
#CXXFLAGS = -O

gtkincludedirs = -I/usr/lib/glib/include -I/usr/X11R6/include
libdirs = -L/usr/lib -L/usr/X11R6/lib
libraries = -lgtk -lgdk -lglib -lXext -lX11 -lm

objects = Vector2D.o gui.o RealTimeBattle.o Arena.o Error.o Shape.o MovingObject.o Extras.o

all: RealTimeBattle $(ROBOTDIR)/empty TAGS

RealTimeBattle: $(objects)
	g++ $(CXXFLAGS) -o RealTimeBattle $(objects) $(gtkincludedirs) $(libdirs) $(libraries)


#implicit rule for c-file:   N.o: $(CC) -c N.c $(CPPFLAGS) $(CFLAGS) 
#implicit rule for c++-file: N.o: $(CXX) -c N.cc $(CPPFLAGS) $(CXXFLAGS)

Vector2D.o: Vector2D.cc Vector2D.h
	g++ $(CXXFLAGS) -c Vector2D.cc

gui.o: gui.cc gui.h
RealTimeBattle.o: RealTimeBattle.cc Vector2D.h Arena.h messagetypes.h gui.h
Arena.o: Arena.cc Arena.h messagetypes.h Vector2D.h
Error.o: Error.cc Arena.h messagetypes.h Vector2D.h
MovingObject.o: MovingObject.cc Arena.h messagetypes.h Vector2D.h
Shape.o: Shape.cc Arena.h messagetypes.h Vector2D.h
Extras.o: Extras.cc Arena.h messagetypes.h Vector2D.h
$(ROBOTDIR)/empty: 

#$(ROBOTDIR)/empty: $(ROBOTDIR)/empty.o
#	cd $(ROBOTDIR); \
#	gcc $(coptions) -o empty empty.o; \
#	cd ..

#$(ROBOTDIR)/empty.o: $(ROBOTDIR)/empty.c
#	cd $(ROBOTDIR); \
#	gcc $(coptions) -c -o empty.o empty.c; \
#	cd ..

clean:
	rm -f *~ *.o core* $(ROBOTDIR)/*~ $(ROBOTDIR)/*.o $(ROBOTDIR)/core*

ETAGS:
	etags *.cc *.h
