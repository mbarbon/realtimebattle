noinst_LIBRARIES = libRTB_Gadgets.a

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/src \
	        $(PROFFLAGS) $(THREAD_FLAGS) \
           -DROBOTDIR='"$(ROBOTDIR)"' -DARENADIR='"$(ARENADIR)"' \
           -DRTB_LOCALEDIR='"$(RTB_LOCALEDIR)"'


libRTB_Gadgets_a_SOURCES = \
	AllGadgets.h \
	ArenaGadget.h \
	CommunicationGadget.cc CommunicationGadget.h \
	Function.h \
	Gadget.cc Gadget.h \
	GadgetSet.cc GadgetSet.h \
	RobotBodyGadget.cc RobotBodyGadget.h \
	ShotGadget.cc ShotGadget.h \
	Variable.cc Variable.h \
	WeaponGadget.h WeaponGadget.cc
