export DEBUG_MODE = no
export PAPER_SIZE = a4

SRCDIR=src
ROBOTDIR=Robots
DOCSDIR=Documentation
HTMLDIR=HTML
ARENADIR=Arenas

BINDIR=/usr/bin
INFODIR=/usr/info
INSTALLDIR=/usr/games/RealTimeBattle

target: RealTimeBattle robots
all: RealTimeBattle robots docs ETAGS

RealTimeBattle:
	cd $(SRCDIR) && $(MAKE)

robots:
	cd $(ROBOTDIR) && $(MAKE)

html_docs:
	cd $(DOCSDIR) && $(MAKE)

docs:
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

tar.gz:
	cd .. && tar -czf RealTimeBattle/RealTimeBattle.tar.gz RealTimeBattle/Makefile RealTimeBattle/BUGS RealTimeBattle/COPYING RealTimeBattle/FAQ RealTimeBattle/INSTALL RealTimeBattle/NEWS RealTimeBattle/README RealTimeBattle/TODO RealTimeBattle/src/*.cc RealTimeBattle/src/*.h RealTimeBattle/src/Makefile RealTimeBattle/Documentation/RealTimeBattle*.html RealTimeBattle/Documentation/RealTimeBattle.dvi RealTimeBattle/Documentation/RealTimeBattle.info RealTimeBattle/Arenas/*.arena RealTimeBattle/Robots/*.c* RealTimeBattle/Robots/Makefile 


install:
	mkdir $(INSTALLDIR) $(INSTALLDIR)/Arenas $(INSTALLDIR)/Robots
	cp $(SRCDIR)/RealTimeBattle $(BINDIR)/RealTimeBattle; \
	cp $(DOCSDIR)/RealTimeBattle.info $(INFODIR)/RealTimeBattle.info: \
	gzip $(INFODIR)/RealTimeBattle.info; \
	cp $(ARENADIR)/*.arena    $(INSTALLDIR)/Arenas/; \
	cp $(ROBOTDIR)/empty      $(INSTALLDIR)/Robots/empty; \
	cp $(ROBOTDIR)/fire       $(INSTALLDIR)/Robots/fire; \
	cp $(ROBOTDIR)/thomastest $(INSTALLDIR)/Robots/thomastest; \
	cp $(ROBOTDIR)/thomas2    $(INSTALLDIR)/Robots/thomas2; \
