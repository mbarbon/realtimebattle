
bin_PROGRAMS = Server 
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/Network/common

Server_SOURCES = \
	ServerChat_Conn.cc \
	ServerNetConnection.cc ServerNetConnection.h \
	ServerPackets.cc \
	ServerRobot_Conn.cc \
	ServerSocket.cc ServerSocket.h

Server_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a
# ../../libRTB.a
Server_LDADD        = $(top_builddir)/src/Network/common/libRTBNetwork.a
# ../../libRTB.a 

