bin_PROGRAMS = RealTimeBattle
include_HEADERS = Messagetypes.h

rtbheaderdir = $(RTB_DIR)
rtbheader_DATA = Messagetypes.h

if NO_GETOPT_LONG
GETOPT_O = getopt.o
else
GETOPT_O =
endif

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
CXXFLAGS += $(GTK_CFLAGS) -I$(top_srcdir)/intl -DROBOTDIR='"$(ROBOTDIR)"'
CXXFLAGS += -DARENADIR='"$(ARENADIR)"' -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'
else
CCMALLOC_O =
CXXFLAGS += $(GTK_CFLAGS) -I$(top_srcdir)/intl -DROBOTDIR='"$(ROBOTDIR)"'
CXXFLAGS += -DARENADIR='"$(ARENADIR)"' -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'
endif


if ENABLE_PROF
CXXFLAGS += -pg
CFLAGS += -pg
endif

if NO_GRAPHICS
CXXFLAGS += -DNO_GRAPHICS 
endif

RealTimeBattle_LDADD = $(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) 
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(INTLOBJS)
RealTimeBattle_SOURCES = \
	ArenaBase.cc ArenaBase.h \
	ArenaController.cc ArenaController.h \
	ArenaRealTime.cc ArenaRealTime.h \
	ArenaReplay.h ArenaReplay.cc \
	Extras.cc Extras.h \
	GeometricalObjects.cc GeometricalObjects.h \
	IntlDefs.h \
	List.cc List.h \
	Messagetypes.h \
	MovingObject.h \
	Options.cc Options.h \
	Robot.cc Robot.h \
	RealTimeBattle.cc \
	Shape.cc Shape.h \
	Shot.cc Shot.h \
	String.cc String.h \
	Structs.h \
	Timer.cc Timer.h \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h \
	Wall.h 


EXTRA_RealTimeBattle_SOURCES = \
	getopt.c getopt.h

