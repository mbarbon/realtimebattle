export DEBUG_MODE = no
export PAPER_SIZE = a4

#
# Install directories. Feel free to change if you want the 
# files to be installed somewhere else.
#
BINDIR=/usr/bin
INFODIR=/usr/info
INFO_TOPDIR=$(INFODIR)/dir
INSTALLDIR=/usr/games/RealTimeBattle


# Internal directories, do not change
SRCDIR=src
ROBOTDIR=Robots
DOCSDIR=Documentation
HTMLDIR=HTML
ARENADIR=Arenas

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

clean: root_clean rtb_clean robot_clean doc_clean html_clean

root_clean:
	rm -f *~ $(ARENADIR)/*~ RealTimeBattle.tar.gz BUGS FAQ INSTALL NEWS README TODO

rtb_clean:
	cd $(SRCDIR) && $(MAKE) clean

robot_clean:
	cd $(ROBOTDIR) && $(MAKE) clean

doc_clean:
	cd $(DOCSDIR) && $(MAKE) clean

html_clean:
	cd $(HTMLDIR) && $(MAKE) clean

ETAGS:
	cd $(SRCDIR) && $(MAKE) ETAGS

tar.gz:
	cd .. && tar -czf RealTimeBattle/RealTimeBattle.tar.gz \
	RealTimeBattle/Makefile \
	RealTimeBattle/BUGS \
	RealTimeBattle/COPYING \
	RealTimeBattle/FAQ \
	RealTimeBattle/INSTALL \
	RealTimeBattle/NEWS \
	RealTimeBattle/README \
	RealTimeBattle/TODO \
	RealTimeBattle/VERSION \
	RealTimeBattle/RealTimeBattle.xpm \
	RealTimeBattle/$(SRCDIR)/*.cc \
	RealTimeBattle/$(SRCDIR)/*.h \
	RealTimeBattle/$(SRCDIR)/Makefile \
	RealTimeBattle/$(DOCSDIR)/RealTimeBattle*.html \
	RealTimeBattle/$(DOCSDIR)/*.gif \
	RealTimeBattle/$(DOCSDIR)/RealTimeBattle.dvi \
	RealTimeBattle/$(DOCSDIR)/RealTimeBattle.info \
	RealTimeBattle/$(ARENADIR)/*.arena \
	RealTimeBattle/$(ROBOTDIR)/*.c* \
	RealTimeBattle/$(ROBOTDIR)/Makefile 


install:
	mkdir $(INSTALLDIR) $(INSTALLDIR)/$(ARENADIR) $(INSTALLDIR)/$(ROBOTDIR)
	cp $(SRCDIR)/RealTimeBattle $(BINDIR)/RealTimeBattle; \
	cp $(DOCSDIR)/RealTimeBattle.info $(INFODIR)/RealTimeBattle.info; \
	gzip $(INFODIR)/RealTimeBattle.info; \
	cp $(ARENADIR)/*.arena    $(INSTALLDIR)/$(ARENADIR)/; \
	cp $(ROBOTDIR)/*.robot    $(INSTALLDIR)/$(ROBOTDIR)/; \
	cp $(SRCDIR)/Messagetypes.h   $(INSTALLDIR)/; \
	cp RealTimeBattle.xpm   $(INSTALLDIR)/; \
	grep -e "RealTimeBattle" $(INFO_TOPDIR); \
	if [ $? -eq 1 ]; then echo -e "\n* RealTimeBattle: (RealTimeBattle).             A robot programming game\n" >> $(INFO_TOPDIR); fi

rpm-install:
	$(MAKE) -f Makefile.rpm rpm-install

rpm: tar.gz
	$(MAKE) -f Makefile.rpm rpm

website: html_docs
	cd $(HTMLDIR) && $(MAKE)

tar-website: 
	cd $(HTMLDIR) && $(MAKE) tar-website


