SUBDIRS = Gui Network

bin_PROGRAMS = RealTimeBattle
include_HEADERS = Messagetypes.h

rtbheaderdir = $(RTB_DIR)
rtbheader_DATA = Messagetypes.h

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl $(PROFFLAGS) $(THREAD_FLAGS) \
           -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"' \
           -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'

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
RealTimeBattle_LDADD = $(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) \
	$(STDCXX_LIBS) $(THREAD_LIBS)
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(INTLOBJS) $(GUI_OBJS)
RealTimeBattle_SOURCES = \
	ArenaBase.cc ArenaBase.h \
	ArenaController.cc ArenaController.h \
	ArenaRealTime.cc ArenaRealTime.h \
	ArenaReplay.h ArenaReplay.cc \
	Event.h \
	EventHandler.h EventHandler.cc \
	Extras.cc Extras.h \
	Function.h \
	Gadget.h \
	GuiEvent.h \
	GuiEventHandler.cc GuiEventHandler.h \
	GeometricalObjects.cc GeometricalObjects.h \
	GuiInterface.cc GuiInterface.h \
	IntlDefs.h \
	List.cc List.h \
	Messagetypes.h \
	MovingObject.h \
	Option.cc Option.h \
	OptionHandler.cc OptionHandler.h \
	Process.cc Process.h \
	Robot.cc Robot.h \
	RollingObject.cc RollingObject.h \
	RealTimeBattle.cc \
	Shape.cc Shape.h \
	Shot.cc Shot.h \
	String.cc String.h \
	Structs.h \
	Timer.cc Timer.h \
	Variable.cc Variable.h \
	Various.cc Various.h \
	Vector2D.cc Vector2D.h \
	Wall.h \
	WeaponGadget.h


EXTRA_RealTimeBattle_SOURCES = \
	getopt.c getopt.h

