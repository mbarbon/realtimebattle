export DEBUG_MODE = yes

SRCDIR=src
ROBOTDIR=Robots
DOCSDIR=Documentation
HTMLDIR=HTML
ARENADIR=Arenas

target: RealTimeBattle robots documentation
all: RealTimeBattle robots all_documentation ETAGS

RealTimeBattle:
	cd $(SRCDIR) && $(MAKE)

robots:
	cd $(ROBOTDIR) && $(MAKE)

documentation:
	cd $(DOCSDIR) && $(MAKE)

all_documentation:
	cd $(DOCSDIR) && $(MAKE) all

clean: root_clean rtb_clean robot_clean doc_clean

root_clean:
	rm -f *~ $(HTMLDIR)/*~ $(ARENADIR)/*~

rtb_clean:
	cd $(SRCDIR) && $(MAKE) clean

robot_clean:
	cd $(ROBOTDIR) && $(MAKE) clean

doc_clean:
	cd $(DOCSDIR) && $(MAKE) clean

ETAGS:
	cd $(SRCDIR) && $(MAKE) ETAGS

tgz:
	tar -czf RealTimeBattle.tgz Makefile COPYING src/*.cc src/*.h src/Makefile Documentation/RealTimeBattle*.html Documentation/RealTimeBattle.dvi Documentation/RealTimeBattle.info Arenas/*.arena Robots/*.c* Robots/Makefile


install:
#NOT YET DONE!!!