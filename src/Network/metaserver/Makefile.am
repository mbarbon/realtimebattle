
bin_PROGRAMS = metaserver 
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/Network/common

metaserver_SOURCES = \
		MetaServerNetConnection.cc \
		MetaServerPackets.cc \
		MetaServerSocket.cc MetaServerSocket.h


metaserver_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a
# ../../libRTB.a
metaserver_LDADD        = $(top_builddir)/src/Network/common/libRTBNetwork.a
# ../../libRTB.a

