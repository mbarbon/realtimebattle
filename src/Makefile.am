SUBDIRS = Gui

bin_PROGRAMS = RealTimeBattle
include_HEADERS = Messagetypes.h

rtbheaderdir = $(RTB_DIR)
rtbheader_DATA = Messagetypes.h

if ENABLE_PROF
PROFFLAGS = -pg
endif

if NO_GRAPHICS
GRAPHFLAGS = -DNO_GRAPHICS 
endif

INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir)/intl \
			  $(PROFFLAGS) $(GRAPHFLAGS) $(THREAD_FLAGS) \
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
RealTimeBattle_LDADD = $(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) $(THREAD_LIBS) $(top_srcdir)/src/Gui/Gtk/.libs/libGuiGtk.so 
RealTimeBattle_DEPENDENCIES = $(GETOPT_O) $(INTLOBJS) $(top_srcdir)/src/Gui/Gtk/.libs/libGuiGtk.so
RealTimeBattle_SOURCES = \
	ArenaBase.cc ArenaBase.h \
	ArenaController.cc ArenaController.h \
	ArenaRealTime.cc ArenaRealTime.h \
	ArenaReplay.h ArenaReplay.cc \
	Extras.cc Extras.h \
	GeometricalObjects.cc GeometricalObjects.h \
	GuiInterface.cc GuiInterface.h \
	IntlDefs.h \
	List.cc List.h \
	Messagetypes.h \
	MovingObject.h \
	Options.cc Options.h \
	Process.cc Process.h \
	Robot.cc Robot.h \
	RollingObject.cc RollingObject.h \
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

