lib_LTLIBRARIES = libGui.la

INCLUDES = $(GUI_FLAGS) -I$(top_srcdir)/intl -I$(top_srcdir)/src \
           $(PROFFLAGS) $(THREAD_FLAGS)

if ENABLE_CCMALLOC
CCMALLOC_O = /usr/lib/ccmalloc.o -ldl
else
CCMALLOC_O =
endif

libGui_la_LIBADD =
libGui_la_LDFLAGS = -version-info  $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	$(INTLLIBS)
libGui_la_DEPENDENCIES = $(INTLOBJS)
libGui_la_SOURCES = Gui.cc