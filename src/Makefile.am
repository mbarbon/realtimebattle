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
GRAPHICS_O = \
	Gui.o ControlWindow.o MessageWindow.o \
	Statistics.o StartTournamentWindow.o Structs.o
endif

RealTimeBattle_LDADD = $(GETOPT_O) $(GRAPHICS_O) $(GTK_LIBS)
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(GRAPHICS_O)
RealTimeBattle_SOURCES = \
	Arena_Base.cc Arena_Base.h \
	Arena_RealTime.cc Arena_RealTime.h \
	Arena_Replay.h Arena_Replay.cc \
	Arena_Controller.cc Arena_Controller.h \
	Extras.cc Extras.h \
	Gui.h \
	Messagetypes.h \
	MovingObject.h \
	Options.cc Options.h \
	RealTimeBattle.cc \
	Shape.cc Shape.h \
	String.cc String.h \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h \
	GeometricalObjects.cc GeometricalObjects.h \
	Robot.cc Robot.h \
	Shot.cc Shot.h \
	Structs.h \
	Timer.cc Timer.h \
	Wall.h 


EXTRA_RealTimeBattle_SOURCES = \
	ControlWindow.cc ControlWindow.h \
	getopt.c getopt.h \
	Gui.cc \
	MessageWindow.cc MessageWindow.h \
	StartTournamentWindow.cc StartTournamentWindow.h \
	Statistics.cc \
	Structs.cc

