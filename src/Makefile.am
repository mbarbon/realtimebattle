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
	Gui.o ArenaWindow.o \
	ControlWindow.o MessageWindow.o \
	OptionsWindow.o \
	ScoreWindow.o StatisticsWindow.o \
	StartTournamentWindow.o Structs.o
endif

RealTimeBattle_LDADD = $(GETOPT_O) $(GRAPHICS_O) $(GTK_LIBS)
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(GRAPHICS_O)
RealTimeBattle_SOURCES = \
	Arena_Base.cc Arena_Base.h \
	Arena_Controller.cc Arena_Controller.h \
	Arena_RealTime.cc Arena_RealTime.h \
	Arena_Replay.h Arena_Replay.cc \
	Extras.cc Extras.h \
	List.cc List.h \
	GeometricalObjects.cc GeometricalObjects.h \
	Messagetypes.h \
	MovingObject.h \
	Options.cc Options.h \
	RealTimeBattle.cc \
	Shape.cc Shape.h \
	String.cc String.h \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h \
	Robot.cc Robot.h \
	Shot.cc Shot.h \
	Structs.h \
	Timer.cc Timer.h \
	Wall.h 


EXTRA_RealTimeBattle_SOURCES = \
	ArenaWindow.cc ArenaWindow.h \
	ControlWindow.cc ControlWindow.h \
	getopt.c getopt.h \
	Gui.cc Gui.h \
	MessageWindow.cc MessageWindow.h \
	OptionsWindow.cc OptionsWindow.h \
	ScoreWindow.cc ScoreWindow.h \
	StartTournamentWindow.cc StartTournamentWindow.h \
	StatisticsWindow.cc StatisticsWindow.h \
	Structs.cc

