noinst_LIBRARIES = libRTB_Gadgets.a

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/src \
   $(PROFFLAGS) $(THREAD_FLAGS)


libRTB_Gadgets_a_SOURCES = \
	AllGadgets.h \
	ArenaGadget.h \
	CommunicationGadget.cc CommunicationGadget.h \
	DefenseGadget.cc DefenseGadget.h \
	EngineGadget.cc EngineGadget.h \
	ExplosionGadget.cc ExplosionGadget.h \
	Function.h \
	Gadget.cc Gadget.h \
	GadgetSet.cc GadgetSet.h \
	RobotBodyGadget.cc RobotBodyGadget.h \
	Script.h Script.cc \
	ShotGadget.cc ShotGadget.h \
	Value.h Value.cc \
	Variable.cc Variable.h \
	WeaponGadget.h WeaponGadget.cc
