bin_PROGRAMS = RealTimeBattle
noinst_LIBRARIES = libRTB_Core.a

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

RealTimeBattle_LDFLAGS = -export-dynamic
RealTimeBattle_LDADD = \
	$(top_builddir)/src/Core/libRTB_Core.a \
	$(top_builddir)/src/Gadgets/libRTB_Gadgets.a \
	$(top_builddir)/src/Network/libRTB_Network.a \
	$(top_builddir)/src/Common/libRTB_Common.a \
	$(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) $(STDCXX_LIBS) $(THREAD_LIBS)

RealTimeBattle_DEPENDENCIES =  \
	$(top_builddir)/src/Core/libRTB_Core.a \
	$(top_builddir)/src/Gadgets/libRTB_Gadgets.a \
	$(top_builddir)/src/Network/libRTB_Network.a \
	$(top_builddir)/src/Common/libRTB_Common.a \
	$(GETOPT_O) $(INTLDEPS) $(GUI_OBJS)
RealTimeBattle_SOURCES = RealTimeBattle.cc


if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl \
	-I$(top_srcdir)/src/Gadgets/ \
	-I$(top_srcdir)/src/Common/ \
	-I$(top_srcdir)/src/Network/ \
	$(PROFFLAGS) $(THREAD_FLAGS)


libRTB_Core_a_SOURCES = \
	Arena.cc            EventRT.h          SubSquare.h \
	ArenaController.cc  SubSquareIterators.cc \
	ArenaController.h   Makefile.am        SubSquareIterators.h \
	Arena.h             Team.cc \
	Robot.cc            Team.h \
	EventGT.cc          Robot.h            Timer.cc \
	EventGT.h           Rotation.cc        Timer.h \
	Event.h             Rotation.h         Tournament.cc \
	EventHandler.cc     Tournament.h \
	EventHandler.h      \
	EventRT.cc          SubSquare.cc


