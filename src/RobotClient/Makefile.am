bin_PROGRAMS = rtb_RobotClient

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


rtb_RobotClient_LDADD = $(top_builddir)/src/Network/libRTB_Network.a 
	$(GETOPT_O) $(INTLLIBS) $(CCMALLOC_O) $(STDCXX_LIBS) 

#rtb_RobotClient_LDFLAGS =  -export-dynamic

rtb_RobotClient_DEPENDENCIES = $(INTLOBJS) $(top_builddir)/src/Network/libRTB_Network.a	$(GETOPT_O) $(INTLDEPS) $(GUI_OBJS)
rtb_RobotClient_SOURCES = \
	RobotClient.cc RobotClient.h \
	RobotPacketFactory.h RobotPacketFactory.cc
