#CC = gcc
#CXX = g++

# Add '-fhandle-exceptions' to CXXFLAGS and remove '-O' from OPTIMIZEFLAGS if your compiler requires it.
# Add '-DUSE_STDARG_H' if the compiler complains about va_list, va_args or similar.
# Add '-DNO_RLIMIT_NPROC' if the compiler complains about RLIMIT_NPROC.
# Add '-DUSE_SYS_TIME_H' if the compiler complains about 'timeval'.
CXXFLAGS =-DINSTALLDIR='"$(INSTALLDIR)"'

OPTIMIZEFLAGS = -O

ifdef DEBUG
OPTIMIZEFLAGS = -g -Wall
endif

CPPFLAGS = $(gtkincludedirs)

ifndef RPM_OPT_FLAGS
CFLAGS = $(OPTIMIZEFLAGS)
CXXFLAGS += $(OPTIMIZEFLAGS)
else
CFLAGS = $(RPM_OPT_FLAGS)
CXXFLAGS += $(RPM_OPT_FLAGS)
endif

gtkincludedirs = `$(gtk_dir)gtk-config --cflags`
libs = `$(gtk_dir)gtk-config --libs`

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
