bin_PROGRAMS = RTBserver RTBclient

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = $(PROFFLAGS) -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"' \
           -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif



RTBserver_DEPENDENCIES = $(INTLOBJS)
RTBserver_SOURCES = \
	Server.cc \
	Socklib.c Socklib.h


RTBclient_DEPENDENCIES = $(INTLOBJS)
RTBclient_SOURCES = \
	Client.cc \
	SimpleProcess.cc SimpleProcess.h \
	Socklib.c Socklib.h
