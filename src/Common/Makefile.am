noinst_LIBRARIES = libRTB_Common.a

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl \
	-I$(top_srcdir)/src/Gadgets/ \
	-I$(top_srcdir)/src/Core/ \
	-I$(top_srcdir)/src/Network/ \
	$(PROFFLAGS) $(THREAD_FLAGS)


libRTB_Common_a_SOURCES = \
	Console.cc                 IntlDefs.h        Shape.cc \
	Console.h                  Shape.h \
	Messagetypes.h    String.cc \
	Explosion.cc               MovingObject.h    String.h \
	Explosion.h                Option.cc        \
	GeometricalObjects.cc      Option.h          Structs.h \
	GeometricalObjects.h       OptionHandler.cc  Various.cc \
	getopt.c                   OptionHandler.h   Various.h \
	getopt.h                   Process.cc        Vector2D.cc \
	InfoClasses.h              Process.h         Vector2D.h \
	InformationDistributor.cc  RollingObject.cc  Wall.h \
	InformationDistributor.h   RollingObject.h  \
	Shot.cc			   Shot.h


