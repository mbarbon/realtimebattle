SUBDIRS = Gadgets Network Gui

bin_PROGRAMS = RealTimeBattle
include_HEADERS = Messagetypes.h

rtbheaderdir = $(RTB_DIR)
rtbheader_DATA = Messagetypes.h

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/src/Gadgets \
   $(PROFFLAGS) $(THREAD_FLAGS)

if NO_GETOPT_LONG
GETOPT_O = getopt.o
else
GETOPT_O =
endif

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif

#Note the gui-lib should be taken via configure
RealTimeBattle_LDFLAGS = -export-dynamic
RealTimeBattle_LDADD = $(top_srcdir)/src/Gadgets/libRTB_Gadgets.a \
	 $(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) $(STDCXX_LIBS) $(THREAD_LIBS)
RealTimeBattle_DEPENDENCIES = $(top_srcdir)/src/Gadgets/libRTB_Gadgets.a \
	$(GETOPT_O) $(INTLOBJS) $(GUI_OBJS)
RealTimeBattle_SOURCES = \
	Arena.cc Arena.h \
	ArenaController.cc ArenaController.h \
	Event.h EventGT.h EventRT.h \
	EventHandler.h EventHandler.cc \
	Explosion.h Explosion.cc \
	Extras.cc Extras.h \
	GeometricalObjects.cc GeometricalObjects.h \
	GuiInterface.cc GuiInterface.h \
	GuiRequest.cc GuiRequest.h \
	InfoClasses.h \
	InformationDistributor.cc InformationDistributor.h \
	IntlDefs.h \
	Messagetypes.h \
	MovingObject.h \
	Option.cc Option.h \
	OptionHandler.cc OptionHandler.h \
	Process.cc Process.h \
	RealTimeBattle.cc \
	Robot.cc Robot.h \
	RollingObject.cc RollingObject.h \
	Shape.cc Shape.h \
	Shot.cc Shot.h \
	String.cc String.h \
	Structs.h \
	SubSquare.h SubSquare.cc \
	Team.cc Team.h \
	Timer.cc Timer.h \
	Tournament.h Tournament.cc \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h \
	Wall.h


EXTRA_RealTimeBattle_SOURCES = \
	getopt.c getopt.h

