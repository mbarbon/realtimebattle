robotexecdir = $(ROBOTDIR)
CXXFLAGS += $(GTK_CFLAGS) -I$(top_srcdir)/src/
robotexec_PROGRAMS = seek_and_destroy.robot
seek_and_destroy_robot_LDADD = $(GTK_LIBS)
seek_and_destroy_robot_SOURCES = seek_and_destroy.cc

saddir = $(ROBOTDIR)/seek_and_destroy
sad_DATA = $(seek_and_destroy_robot_SOURCES) Makefile.am 

