noinst_LIBRARIES = libGuiGtk.a

INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir)/intl -I$(top_srcdir)/src

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
CXXFLAGS += -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"'
CXXFLAGS += -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'
else
CCMALLOC_O =
CXXFLAGS += -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"'
CXXFLAGS += -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'
endif

if ENABLE_PROF
CXXFLAGS += -pg
CFLAGS += -pg
endif

libGuiGtk_a_LIBADD = $(GTK_LIBS) $(INTLLIBS)
libGuiGtk_a_DEPENDENCIES = $(INTLOBJS)
libGuiGtk_a_SOURCES = \
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
