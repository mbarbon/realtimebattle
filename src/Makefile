#CC = gcc
#CXX = g++

ifndef DEBUG_MODE
DEBUG_MODE=no
endif

# Add '-fhandle-exception' to CXXFLAGS if your compiler requires it.
CXXFLAGS =


ifndef RPM_OPT_FLAGS
ifeq ($(DEBUG_MODE), yes)
#debug-options
CPPFLAGS = $(gtkincludedirs)
CFLAGS = -g -Wall
CXXFLAGS += -g -Wall
else
#final-options
CPPFLAGS = $(gtkincludedirs)
CFLAGS = -O
CXXFLAGS += -O
endif
else
CPPFLAGS = $(gtkincludedirs)
CFLAGS = $(RPM_OPT_FLAGS)
CXXFLAGS += $(RPM_OPT_FLAGS)
endif

gtkincludedirs = `gtk-config --cflags`
libs = `gtk-config --libs`

objects = Vector2D.o Gui.o RealTimeBattle.o Arena.o Error.o Shape.o MovingObject.o Extras.o Options.o Statistics.o StartTournament.o Various.o String.o
stdincludes = Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h Error.h String.h Shape.h Extras.h MovingObject.h

target: RealTimeBattle

RealTimeBattle: $(objects)
	$(CXX) $(CXXFLAGS) -o RealTimeBattle $(objects) $(gtkincludedirs) $(libs)


#implicit rule for c-file:   N.o: $(CC) -c N.c $(CPPFLAGS) $(CFLAGS) 
#implicit rule for c++-file: N.o: $(CXX) -c N.cc $(CPPFLAGS) $(CXXFLAGS)

String.o: String.cc String.h
Vector2D.o: Vector2D.cc Vector2D.h
Various.o: Various.cc Various.h Error.h String.h
Error.o: Error.cc Error.h String.h
Gui.o: Gui.cc $(stdincludes)
RealTimeBattle.o: RealTimeBattle.cc Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h Error.h String.h
Arena.o: Arena.cc $(stdincludes)
MovingObject.o: MovingObject.cc $(stdincludes)
Shape.o: Shape.cc Shape.h Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h String.h
Extras.o: Extras.cc Extras.h Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h String.h Shape.h
Options.o: Options.cc Options.h Vector2D.h Gui.h Various.h Arena.h Messagetypes.h String.h
Statistics.o: Statistics.cc Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h String.h Shape.h MovingObject.h
StartTournament.o: StartTournament.cc Vector2D.h Gui.h Various.h Options.h Arena.h Messagetypes.h String.h

clean:
	rm -f *~ RealTimeBattle *.o core*

ETAGS:
	etags *.cc *.h
