noinst_LIBRARIES = libRTBNetClients.a

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/Network/common

libRTBNetClients_a_SOURCES = ClientSocket.cc ClientSocket.h
libRTBNetClients_a_DEPENDENCIES = $(top_builddir)/src/Network/common/libRTBNetwork.a
libRTBNetClients_a_LDADD = $(top_builddir)/src/Network/common/libRTBNetwork.a
