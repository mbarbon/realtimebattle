noinst_LIBRARIES = libRTB_Gadgets.a

if ENABLE_PROF
PROFFLAGS = -pg
endif

INCLUDES = -I$(top_srcdir)/intl -I$(top_srcdir)/src -I$(top_srcdir)/src/Network/ \
   $(PROFFLAGS) $(THREAD_FLAGS)


libRTB_Gadgets_a_SOURCES = \
	AllGadgets.cc AllGadgets.h \
	ArenaGadget.h \
	CommunicationGadget.cc CommunicationGadget.h \
	DefenseGadget.cc DefenseGadget.h \
	EngineGadget.cc EngineGadget.h \
	EnvironmentGadget.h EnvironmentGadget.cc \
	ExplosionGadget.cc ExplosionGadget.h \
	Function.h \
	Gadget.cc Gadget.h \
	GadgetSet.cc GadgetSet.h \
	RobotBodyGadget.cc RobotBodyGadget.h \
	Script.h Script.cc \
	SensorGadget.cc SensorGadget.h \
	ShotGadget.cc ShotGadget.h \
	Value.h Value.cc \
	Variable.cc Variable.h \
	WallGadget.h WallGadget.cc \
	WeaponGadget.h WeaponGadget.cc
