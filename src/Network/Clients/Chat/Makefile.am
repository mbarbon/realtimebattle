
bin_PROGRAMS = Chat Root
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/Network/common \
	-I$(top_srcdir)/src/Network/Clients/common

Chat_SOURCES = ChatPackets.cc mainChat.cc

Chat_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a
Chat_LDADD        = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a 

Root_SOURCES = ChatPackets.cc mainRoot.cc

Root_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a
Root_LDADD        = $(top_builddir)/src/Network/common/libRTBNetwork.a \
	$(top_builddir)/src/Network/Clients/common/libRTBNetClients.a
# ../../../libRTB.a 

