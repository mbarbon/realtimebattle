

#
# Install directories. Feel free to change if you want the 
# files to be installed somewhere else.
#
BINDIR=/usr/bin
INFODIR=/usr/info
INFO_TOPDIR=$(INFODIR)/dir
export INSTALLDIR=/usr/games/RealTimeBattle
INCLUDEDIR=/usr/include


# Internal directories, do not change
SRCDIR=src
ROBOTDIR=Robots
DOCSDIR=Documentation
HTMLDIR=HTML
ARENADIR=Arenas

#If you compile the documentation from the sgml-sources with, you may change the paper-size to 'letter'.
#Mainly for the author of the documentation.
export PAPER_SIZE = a4

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

clean: root_clean rtb_clean robot_clean

clean_all: root_clean_all rtb_clean robot_clean docs_clean html_clean

root_clean:
	rm -f *~ $(ARENADIR)/*~ core*

root_clean_all:
	rm -f *~ RealTimeBattle*.rpm $(ARENADIR)/*~ RealTimeBattle.tar* core* BUGS FAQ INSTALL ChangeLog README TODO


rtb_clean:
	cd $(SRCDIR) && $(MAKE) clean

robot_clean:
	cd $(ROBOTDIR) && $(MAKE) -k clean

docs_clean:
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

tar.gz: 
	$(MAKE) tar; \
	gzip -f RealTimeBattle.tar

tar.bz2: 
	$(MAKE) tar; \
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
   install-info --entry "* RealTimeBattle: (RealTimeBattle).             A robot programming game" \
                --info-dir $(INFODIR) $(INFODIR)/RealTimeBattle.info.gz

uninstall:
	rm -r $(INSTALLDIR); \
	rm $(INCLUDEDIR)/Messagetypes.h; \
	rm $(BINDIR)/RealTimeBattle; \
   install-info --delete --info-dir $(INFODIR) $(INFODIR)/RealTimeBattle.info.gz; \
	rm $(INFODIR)/RealTimeBattle.info.gz

rpm-install:
	$(MAKE) -f Makefile.rpm rpm-install

rpm: tar.gz
	$(MAKE) -f Makefile.rpm rpm

website: html_docs
	cd $(HTMLDIR) && $(MAKE)

tar-website: 
	cd $(HTMLDIR) && $(MAKE) tar-website

website_download: tar.bz2 tar.gz docs
	cd $(HTMLDIR) && $(MAKE) website_download

website_all: html_clean website website_download
