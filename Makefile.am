SUBDIRS = intl po Arenas Robots Documentation src 

rtbstddocsdir = $(DOCS_DIR)
rtbstddocs_DATA = ABOUT-NLS BUGS FAQ TODO README INSTALL ChangeLog CVS-Help

rtbextradir = $(RTB_DIR)
rtbextra_DATA = RealTimeBattle.xpm tournament.txt

EXTRA_DIST = $(rtbextra_DATA) $(rtbstddocs_DATA) RealTimeBattle.spec

AUTOMAKE_OPTIONS = foreign


uninstall-local:
	rmdir $(RTB_DIR)/Arenas || true; \
	rmdir -p $(RTB_DIR)/Robots/* || true



#These targets are indended mainly for the maintainer

robots:
	cd Robots && $(MAKE)

html_docs:
	cd Documentation && $(MAKE) html_docs

docs:
	cd Documentation && $(MAKE) docs

bz2_dist: dist
	gunzip -c $(PACKAGE)-$(VERSION).tar.gz | bzip2 > $(PACKAGE)-$(VERSION).tar.bz2

html_clean:
	cd HTML && $(MAKE) -fMakefile.rtb clean

website: html_docs
	cd HTML && $(MAKE) -fMakefile.rtb

tar-website: 
	cd HTML && $(MAKE) -fMakefile.rtb tar-website

website_download: docs bz2_dist
	cd HTML && $(MAKE) -fMakefile.rtb website_download VERSION=$(VERSION)

website_all: html_clean website website_download

realclean: clean
	cd Documentation && $(MAKE) realclean

rpm: dist
	cp $(PACKAGE)-$(VERSION).tar.gz $(shell rpm --showrc | grep "^sourcedir" | sed "s/sourcedir[  ]*: *//") && \
	cp RealTimeBattle.spec $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	rpm -ba --clean --rmsource $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	cp $(shell rpm --showrc | grep "^rpmdir" | sed "s/rpmdir[  ]*: *//")/$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//").rpm . && \
	cp $(shell rpm --showrc | grep "^srcrpmdir" | sed "s/srcrpmdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).src.rpm . 

#	sed -e "s/#VERSION#/$(VERSION)/" -e "s/#RELEASE#/$(RTB_RPM_RELEASE)/" RealTimeBattle.spec > $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \


