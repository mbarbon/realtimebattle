SUBDIRS = Arenas Robots Documentation src

rtbstddocsdir = $(DOCS_DIR)
rtbstddocs_DATA = BUGS FAQ TODO README INSTALL ChangeLog

rtbextradir = $(RTB_DIR)
rtbextra_DATA = RealTimeBattle.xpm 

EXTRA_DIST = $(rtbextra_DATA) $(rtbstddocs_DATA)

AUTOMAKE_OPTIONS = foreign

#If you compile the documentation from the sgml-sources with, you may change the paper-size to 'letter'.
#Mainly for the author of the documentation.
export PAPER_SIZE = a4


uninstall-local:
	rmdir $(RTB_DIR)/Arenas; \
	rm -r $(RTB_DIR)/Robots; \
	rmdir $(RTB_DIR)

#These targets are indended mainly for the maintainer

robots:
	cd Robots && $(MAKE)

html_docs:
	cd Documentation && $(MAKE) html_docs

docs:
	cd Documentation && $(MAKE) docs

tar:
	cd .. && tar -cf RealTimeBattle/RealTimeBattle.tar \
	RealTimeBattle/Makefile.{am,in} \
	RealTimeBattle/configure \
	RealTimeBattle/aclocal.m4 \
	RealTimeBattle/install-sh \
	RealTimeBattle/mkinstalldirs \
	RealTimeBattle/configure.in \
	RealTimeBattle/BUGS \
	RealTimeBattle/COPYING \
	RealTimeBattle/FAQ \
	RealTimeBattle/INSTALL \
	RealTimeBattle/ChangeLog \
	RealTimeBattle/README \
	RealTimeBattle/TODO \
	RealTimeBattle/VERSION \
	RealTimeBattle/RealTimeBattle.xpm \
	RealTimeBattle/src/*.cc \
	RealTimeBattle/src/getopt.c \
	RealTimeBattle/src/*.h \
	RealTimeBattle/src/Makefile.{am,in} \
	RealTimeBattle/Documentation/RealTimeBattle*.html \
	RealTimeBattle/Documentation/*.gif \
	RealTimeBattle/Documentation/RealTimeBattle.dvi \
	RealTimeBattle/Documentation/RealTimeBattle.info \
	RealTimeBattle/Documentation/Makefile.{am,in} \
	RealTimeBattle/Arenas/*.arena \
	RealTimeBattle/Arenas/Makefile.{am,in} \
	RealTimeBattle/Robots/Makefile.{am,in} \
	RealTimeBattle/Robots/*/*.c* \
	RealTimeBattle/Robots/*/Makefile.{am,in}

tar.gz: 
	$(MAKE) tar; \
	gzip -f RealTimeBattle.tar

tar.bz2: 
	$(MAKE) tar; \
	bzip2 -f RealTimeBattle.tar

html_clean:
	cd HTML && $(MAKE) -fMakefile.rtb clean

website: html_docs
	cd HTML && $(MAKE) -fMakefile.rtb

tar-website: 
	cd HTML && $(MAKE) -fMakefile.rtb tar-website

website_download: docs tar.bz2 tar.gz
	cd HTML && $(MAKE) -fMakefile.rtb website_download

website_all: html_clean website website_download


rpm-install:
	mkdir /usr/games/RealTimeBattle; \
	cp src/RealTimeBattle /usr/bin/RealTimeBattle; \
	cp -R Arenas   	/usr/games/RealTimeBattle/; \
	cp -R Robots   	/usr/games/RealTimeBattle/; \
	cp src/Messagetypes.h   /usr/games/RealTimeBattle/; \
	cp src/Messagetypes.h   /usr/include/; \
	cp RealTimeBattle.xpm   /usr/games/RealTimeBattle/; \
	gzip -c Documentation/RealTimeBattle.info > $(infodir)/RealTimeBattle.info.gz

rpm: tar.gz
	cp RealTimeBattle.tar.gz $(shell rpm --showrc | grep "^sourcedir" | sed "s/sourcedir[  ]*: *//") && \
	sed -e "s/#VERSION#/$(VERSION)/" -e "s/#RELEASE#/$(RTB_RPM_RELEASE)/" RealTimeBattle.spec > $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	rpm -ba $(shell rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).spec && \
	cp $(shell rpm --showrc | grep "^rpmdir" | sed "s/rpmdir[  ]*: *//")/$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).$(shell rpm --showrc | grep "^build arch" | sed "s/build arch[  ]*: *//").rpm . && \
	cp $(shell rpm --showrc | grep "^srcrpmdir" | sed "s/srcrpmdir[  ]*: *//")/RealTimeBattle-$(RTB_RPM_VERSION).src.rpm . 



