noinst_LIBRARIES = libRTB_Network.a

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/src -I$(top_srcdir)/src/Gadgets/ \
   $(PROFFLAGS) $(THREAD_FLAGS)


libRTB_Network_a_SOURCES = \
	NetConnection.cc    ServerSocket.cc \
	NetConnection.h     ServerSocket.h  \
	Packets.cc          ServerPackets.cc        SimpleProcess.cc\
	Packets.h           ServerPackets.h         SimpleProcess.h\
	TournamentAgreementPackets.cc TournamentAgreementPackets.h \
	TournamentPackets.cc	TournamentPackets.h


