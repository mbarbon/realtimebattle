
bin_PROGRAMS = Robot
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/Network/common \
	-I$(top_srcdir)/src/Network/Clients/common

Robot_SOURCES = RobotPackets.cc  RobotSocket.cc  mainRobot.cc

Robot_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a
Robot_LDADD        = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a 

