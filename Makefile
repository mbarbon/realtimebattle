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

objects = Vector2D.o gui.o RealTimeBattle.o Arena.o Error.o Shape.o MovingObject.o Extras.o Options.o Statistics.o StartTournament.o Various.o String.o
stdincludes = Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h Error.h String.h Shape.h Extras.h MovingObject.h

all: RealTimeBattle robots

RealTimeBattle: $(objects)
	g++ $(CXXFLAGS) -o RealTimeBattle $(objects) $(gtkincludedirs) $(libdirs) $(libraries)


#implicit rule for c-file:   N.o: $(CC) -c N.c $(CPPFLAGS) $(CFLAGS) 
#implicit rule for c++-file: N.o: $(CXX) -c N.cc $(CPPFLAGS) $(CXXFLAGS)

String.o: String.cc String.h
Vector2D.o: Vector2D.cc Vector2D.h
Various.o: Various.cc Various.h Error.h String.h
Error.o: Error.cc Error.h String.h
gui.o: gui.cc $(stdincludes)
RealTimeBattle.o: RealTimeBattle.cc Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h Error.h String.h
Arena.o: Arena.cc $(stdincludes)
MovingObject.o: MovingObject.cc $(stdincludes)
Shape.o: Shape.cc Shape.h Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h String.h
Extras.o: Extras.cc Extras.h Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h String.h Shape.h
Options.o: Options.cc Options.h Vector2D.h gui.h Various.h Arena.h messagetypes.h String.h
Statistics.o: Statistics.cc Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h String.h Shape.h MovingObject.h
StartTournament.o: StartTournament.cc Vector2D.h gui.h Various.h Options.h Arena.h messagetypes.h String.h

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
