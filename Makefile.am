SUBDIRS = Arenas Robots Documentation src

rtbstddocsdir = $(DOCS_DIR)
rtbstddocs_DATA = BUGS FAQ TODO README INSTALL ChangeLog

rtbextradir = $(RTB_DIR)
rtbextra_DATA = RealTimeBattle.xpm

EXTRA_DIST = $(rtbextra_DATA) $(rtbstddocs_DATA) RealTimeBattle.spec

AUTOMAKE_OPTIONS = foreign

#If you compile the documentation from the sgml-sources with, you may change the paper-size to 'letter'.
#Mainly for the author of the documentation.
export PAPER_SIZE = a4


uninstall-local:
	rmdir $(RTB_DIR)/Arenas || true; \
	rmdir -p $(RTB_DIR)/Robots/* || true
#   echo -e "\n** Uninstallation finished! **\n"   


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

website_download: docs tar.bz2 tar.gz
	cd HTML && $(MAKE) -fMakefile.rtb website_download

website_all: html_clean website website_download


rpm: tar.gz
	cp $(distdir).tar.gz $(shell rpm --showrc | grep "^sourcedir" | sed "s/sourcedir[  ]*: *//") && \
	sed -e "s/#VERSION#/$(VERSION)/" -e "s/#RELEASE#/$(RTB_RPM_RELEASE)/" RealTimeBattle.spec > $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	rpm -ba $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	cp $(shell rpm --showrc | grep "^rpmdir" | sed "s/rpmdir[  ]*: *//")/$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//").rpm . && \
	cp $(shell rpm --showrc | grep "^srcrpmdir" | sed "s/srcrpmdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).src.rpm . 



