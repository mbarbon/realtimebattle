lib_LTLIBRARIES = libRTBGui_Gtk.la

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir)/intl -I$(top_srcdir)/src \
   $(PROFFLAGS) $(THREAD_FLAGS)

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif


libRTBGui_Gtk_la_LIBADD = 
libRTBGui_Gtk_la_LDFLAGS = -version-info  $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-module $(STDCXX_LIBS) $(GTK_LIBS) $(CCMALLOC_O)
libRTBGui_Gtk_la_DEPENDENCIES = $(INTLOBJS)
libRTBGui_Gtk_la_SOURCES = \
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
	Window.h
