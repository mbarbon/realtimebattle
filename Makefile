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
INCLUDEDIR=/usr/include


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
	cd $(DOCSDIR) && $(MAKE) html

docs:
	cd $(DOCSDIR) && $(MAKE) all

clean: root_clean rtb_clean robot_clean doc_clean html_clean

root_clean:
	rm -f *~ RealTimeBattle*.rpm $(ARENADIR)/*~ RealTimeBattle.tar* BUGS FAQ INSTALL ChangeLog README TODO

rtb_clean:
	cd $(SRCDIR) && $(MAKE) clean

robot_clean:
	cd $(ROBOTDIR) && $(MAKE) -k clean

doc_clean:
	cd $(DOCSDIR) && $(MAKE) clean

html_clean:
	cd $(HTMLDIR) && $(MAKE) clean

ETAGS:
	cd $(SRCDIR) && $(MAKE) ETAGS

tar: docs
	cd .. && tar -cf RealTimeBattle/RealTimeBattle.tar \
	RealTimeBattle/Makefile \
	RealTimeBattle/Makefile.rpm \
	RealTimeBattle/BUGS \
	RealTimeBattle/COPYING \
	RealTimeBattle/FAQ \
	RealTimeBattle/INSTALL \
	RealTimeBattle/ChangeLog \
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
	RealTimeBattle/$(ROBOTDIR)/Makefile \
	RealTimeBattle/$(ROBOTDIR)/*/*.c* \
	RealTimeBattle/$(ROBOTDIR)/*/Makefile 

tar.gz: tar
	gzip -f RealTimeBattle.tar

tar.bz2: tar
	bzip2 -f RealTimeBattle.tar

install:
	mkdir -p $(INSTALLDIR); \
	cp $(SRCDIR)/RealTimeBattle $(BINDIR)/RealTimeBattle; \
	cp -R $(ARENADIR)    $(INSTALLDIR); \
	cp -R $(ROBOTDIR)    $(INSTALLDIR); \
	cp $(SRCDIR)/Messagetypes.h   $(INSTALLDIR)/; \
	cp $(SRCDIR)/Messagetypes.h   $(INCLUDEDIR)/; \
	cp RealTimeBattle.xpm   $(INSTALLDIR)/; \
	gzip -c $(DOCSDIR)/RealTimeBattle.info > $(INFODIR)/RealTimeBattle.info.gz; \
	cp BUGS FAQ INSTALL ChangeLog README TODO $(INSTALLDIR); \
	cp -R $(DOCSDIR) $(INSTALLDIR); \
	cp Makefile   $(INSTALLDIR)/; \
	grep -qe "RealTimeBattle" $(INFO_TOPDIR) || \
	echo -e "\n* RealTimeBattle: (RealTimeBattle).             A robot programming game\n" >> $(INFO_TOPDIR)

uninstall:
	rm -r $(INSTALLDIR); \
	rm $(INFODIR)/RealTimeBattle.info.gz; \
	rm $(INCLUDEDIR)/Messagetypes.h; \
	rm $(BINDIR)/RealTimeBattle; \
	grep -v "RealTimeBattle" $(INFO_TOPDIR) > /tmp/dir && cat /tmp/dir > $(INFO_TOPDIR)

rpm-install:
	$(MAKE) -f Makefile.rpm rpm-install

rpm: tar.gz
	$(MAKE) -f Makefile.rpm rpm

website: html_docs
	cd $(HTMLDIR) && $(MAKE)

tar-website: 
	cd $(HTMLDIR) && $(MAKE) tar-website


