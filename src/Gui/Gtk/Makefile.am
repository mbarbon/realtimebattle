lib_LTLIBRARIES = libGuiGtk.la

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir)/intl -I$(top_srcdir)/src \
           $(PROFFLAGS) -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"' \
           -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif


libGuiGtk_la_LIBADD = 
libGuiGtk_la_LDFLAGS = -version-info  $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	$(GTK_LIBS) $(INTLLIBS)
libGuiGtk_la_DEPENDENCIES = $(INTLOBJS)
libGuiGtk_la_SOURCES = \
	ArenaWindow.cc ArenaWindow.h \
	ControlWindow.cc ControlWindow.h \
	Dialog.cc Dialog.h \
	DrawingObjects.cc DrawingObjects.h \
	Gui.cc Gui.h \
	GuiInterface.h \
	GuiStructs.cc GuiStructs.h \
	GuiVarious.cc GuiVarious.h \
	MessageWindow.cc MessageWindow.h \
	OptionsWindow.cc OptionsWindow.h \
	ScoreWindow.cc ScoreWindow.h \
	StartTournamentWindow.cc StartTournamentWindow.h \
	StatisticsWindow.cc StatisticsWindow.h
