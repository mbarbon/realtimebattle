ROBOTDIR = Robots

export CC = gcc
export CXX = g++

#debug-options
export CPPFLAGS = $(gtkincludedirs)
export CFLAGS = -g -Wall
export CXXFLAGS = -g -Wall

#normal-options
#export CPPFLAGS = $(gtkincludedirs)
#export CFLAGS = -Wall
#export CXXFLAGS = -Wall

#final-options
#export CPPFLAGS = $(gtkincludedirs)
#export CFLAGS = -O
#export CXXFLAGS = -O

gtkincludedirs = -I/usr/lib/glib/include -I/usr/X11R6/include
export libdirs = -L/usr/lib -L/usr/X11R6/lib
export libraries = -lgtk -lgdk -lglib -lXext -lX11 -lm

objects = Vector2D.o gui.o RealTimeBattle.o Arena.o Error.o Shape.o MovingObject.o Extras.o Options.o Statistics.o

all: RealTimeBattle robots

RealTimeBattle: $(objects)
	g++ $(CXXFLAGS) -o RealTimeBattle $(objects) $(gtkincludedirs) $(libdirs) $(libraries)


#implicit rule for c-file:   N.o: $(CC) -c N.c $(CPPFLAGS) $(CFLAGS) 
#implicit rule for c++-file: N.o: $(CXX) -c N.cc $(CPPFLAGS) $(CXXFLAGS)

Vector2D.o: Vector2D.cc Vector2D.h
	g++ $(CXXFLAGS) -c Vector2D.cc

gui.o: gui.cc gui.h Arena.h Vector2D.h messagetypes.h Options.h
RealTimeBattle.o: RealTimeBattle.cc Vector2D.h Arena.h messagetypes.h gui.h
Arena.o: Arena.cc Arena.h messagetypes.h Vector2D.h gui.h Options.h
Error.o: Error.cc Arena.h messagetypes.h Vector2D.h
MovingObject.o: MovingObject.cc Arena.h messagetypes.h Vector2D.h Options.h gui.h
Shape.o: Shape.cc Arena.h messagetypes.h Vector2D.h Options.h gui.h
Extras.o: Extras.cc Arena.h messagetypes.h Vector2D.h Options.h gui.h
Options.o: Options.cc Options.h
Statistics.o: Statistics.cc gui.h Arena.h Vector2D.h messagetypes.h Options.h

robots:
	cd $(ROBOTDIR) && $(MAKE)

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
