export DEBUG_MODE = yes

SRCDIR=src
ROBOTDIR=Robots
DOCSDIR=Documentation
HTMLDIR=HTML
ARENADIR=Arenas

target: RealTimeBattle robots documentation

RealTimeBattle:
	cd $(SRCDIR) && $(MAKE)

robots:
	cd $(ROBOTDIR) && $(MAKE)

documentation:
	cd $(DOCSDIR) && $(MAKE)

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

install:
#NOT YET DONE!!!