bin_PROGRAMS = RTB_Gtk

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl  \
	-I$(top_srcdir)/src/Network $(PROFFLAGS)

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif
if NO_GETOPT_LONG
GETOPT_O = getopt.o
else
GETOPT_O =
endif


RTB_Gtk_LDADD = $(top_builddir)/src/Network/libRTB_Network.a \
	$(top_builddir)/src/Common/libRTB_Common.a \
	$(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) $(STDCXX_LIBS) $(GTK_LIBS)

RTB_Gtk_LDFLAGS =  -export-dynamic

RTB_Gtk_DEPENDENCIES = $(INTLOBJS) $(top_builddir)/src/Network/libRTB_Network.a	$(GETOPT_O) $(INTLDEPS) $(GUI_OBJS)
RTB_Gtk_SOURCES = \
	ArenaDisplay.cc ArenaDisplay.h \
	ArenaWindow.cc ArenaWindow.h \
	ControlWindow.cc ControlWindow.h \
	Dialog.cc Dialog.h \
	DrawingObjects.cc DrawingObjects.h \
	FileSelector.h \
	Gui.cc Gui.h \
	GuiStructs.cc GuiStructs.h \
	GuiVarious.cc GuiVarious.h \
	MessageWindow.cc MessageWindow.h \
	OptionsWindow.cc OptionsWindow.h \
	ScoreWindow.cc ScoreWindow.h \
	StartTournamentWindow.cc StartTournamentWindow.h \
	StatisticsWindow.cc StatisticsWindow.h \
	Window.cc Window.h
