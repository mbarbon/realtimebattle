CXXFLAGS += $(GTK_CFLAGS) -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"'

bin_PROGRAMS = RealTimeBattle
include_HEADERS = Messagetypes.h

rtbheaderdir = $(RTB_DIR)
rtbheader_DATA = Messagetypes.h

if NO_GETOPT_LONG
GETOPT_O = getopt.o
else
GETOPT_O =
endif

if NO_GRAPHICS
GRAPHICS_O = 
CXXFLAGS += -DNO_GRAPHICS 
else
GRAPHICS_O = Gui.o Statistics.o StartTournament.o
endif

RealTimeBattle_LDADD = $(GETOPT_O) $(GRAPHICS_O) $(GTK_LIBS)
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(GRAPHICS_O)
RealTimeBattle_SOURCES = \
	Arena.cc Arena.h \
	Extras.cc Extras.h \
	Gui.h \
	Messagetypes.h \
	MovingObject.cc MovingObject.h \
	Options.cc Options.h \
	RealTimeBattle.cc \
	Shape.cc Shape.h \
	String.cc String.h \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h

EXTRA_RealTimeBattle_SOURCES = \
	getopt.c getopt.h \
	Gui.cc StartTournament.cc Statistics.cc
