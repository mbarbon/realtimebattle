lib_LTLIBRARIES = libRTBGui_none.la

INCLUDES = $(GUI_FLAGS) -I$(top_srcdir)/intl \
	-I$(top_srcdir)/src $(PROFFLAGS) $(THREAD_FLAGS)

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif

libRTBGui_none_la_LIBADD =
libRTBGui_none_la_LDFLAGS = -version-info  $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-module -export-dynamic $(STDCXX_LIBS) $(INTLLIBS) $(CCMALLOC_O)
libRTBGui_none_la_DEPENDENCIES = $(INTLOBJS)
libRTBGui_none_la_SOURCES = Gui.cc
