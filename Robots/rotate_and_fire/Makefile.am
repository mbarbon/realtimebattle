robotexecdir = $(ROBOTDIR)
CXXFLAGS += -I$(top_srcdir)/src -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'
robotexec_PROGRAMS = rotate_and_fire_signal.robot \
                     rotate_and_fire_blocking.robot \
                     rotate_and_fire_select.robot

rotate_and_fire_signal_robot_LDADD = $(INTLLIBS)
rotate_and_fire_signal_robot_DEPENDENCIES = $(INTLOBJS)
rotate_and_fire_signal_robot_SOURCES = rotate_and_fire.cc rotate_and_fire.h \
                                       rotate_and_fire_signal.cc

rotate_and_fire_blocking_robot_LDADD = $(INTLLIBS)
rotate_and_fire_blocking_robot_DEPENDENCIES = $(INTLOBJS)
rotate_and_fire_blocking_robot_SOURCES = rotate_and_fire.cc rotate_and_fire.h \
                                       rotate_and_fire_blocking.cc

rotate_and_fire_select_robot_LDADD = $(INTLLIBS)
rotate_and_fire_select_robot_DEPENDENCIES = $(INTLOBJS)
rotate_and_fire_select_robot_SOURCES = rotate_and_fire.cc rotate_and_fire.h \
                                       rotate_and_fire_select.cc

rafdir = $(ROBOTDIR)/rotate_and_fire
raf_DATA = $(rotate_and_fire_signal_robot_SOURCES) \
           $(rotate_and_fire_blocking_robot_SOURCES) \
           $(rotate_and_fire_select_robot_SOURCES) \
           Makefile.am