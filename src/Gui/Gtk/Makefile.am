lib_LTLIBRARIES = libGui.la

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = $(GUI_FLAGS) -I$(top_srcdir)/intl -I$(top_srcdir)/src \
           $(PROFFLAGS) $(THREAD_FLAGS) \
			  -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"' \
           -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif


libGui_la_LIBADD = 
libGui_la_LDFLAGS = -version-info  $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	$(GUI_LIBS) $(INTLLIBS)
libGui_la_DEPENDENCIES = $(INTLOBJS)
libGui_la_SOURCES = \
	ArenaWindow.cc ArenaWindow.h \
	ControlWindow.cc ControlWindow.h \
	Dialog.cc Dialog.h \
	DrawingObjects.cc DrawingObjects.h \
	Gui.cc Gui.h \
	GuiStructs.cc GuiStructs.h \
	GuiVarious.cc GuiVarious.h \
	MessageWindow.cc MessageWindow.h \
	OptionsWindow.cc OptionsWindow.h \
	ScoreWindow.cc ScoreWindow.h \
	StartTournamentWindow.cc StartTournamentWindow.h \
	StatisticsWindow.cc StatisticsWindow.h
