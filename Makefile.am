SUBDIRS = Arenas Robots Documentation src

rtbextradir = $(RTB_DIR)

rtbextra_DATA = BUGS FAQ RealTimeBattle.xpm TODO README INSTALL ChangeLog

EXTRA_DIST = $(rtbextra_DATA)


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

tar: docs
	cd .. && tar -cf RealTimeBattle/RealTimeBattle.tar \
	RealTimeBattle/Makefile.am \
	RealTimeBattle/configure \
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
	RealTimeBattle/src/*.h \
	RealTimeBattle/src/Makefile.am \
	RealTimeBattle/Documentation/RealTimeBattle*.html \
	RealTimeBattle/Documentation/*.gif \
	RealTimeBattle/Documentation/RealTimeBattle.dvi \
	RealTimeBattle/Documentation/RealTimeBattle.info \
	RealTimeBattle/Documentation/Makefile.am \
	RealTimeBattle/Arenas/*.arena \
	RealTimeBattle/Arenas/Makefile.am \
	RealTimeBattle/Robots/Makefile.am \
	RealTimeBattle/Robots/*/*.c* \
	RealTimeBattle/Robots/*/Makefile.am

tar.gz: 
	$(MAKE) tar; \
	gzip -f RealTimeBattle.tar

tar.bz2: 
	$(MAKE) tar; \
	bzip2 -f RealTimeBattle.tar


#rpm-install:
#	$(MAKE) -f Makefile.rpm rpm-install

#rpm: tar.gz
#	$(MAKE) -f Makefile.rpm rpm

website: html_docs
	cd HTML && $(MAKE) -fMakefile.rtb

tar-website: 
	cd HTML && $(MAKE) -fMakefile.rtb tar-website

website_download: tar.bz2 tar.gz docs
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
	SOURCEDIR=`rpm --showrc | grep "^sourcedir" | sed "s/sourcedir[  ]*: *//"` && \
	SPECDIR=`rpm --showrc | grep "^specdir" | sed "s/specdir[  ]*: *//"` && \
	RPMDIR=`rpm --showrc | grep "^rpmdir" | sed "s/rpmdir[  ]*: *//"` && \
	SRCRPMDIR=`rpm --showrc | grep "^srcrpmdir" | sed "s/srcrpmdir[  ]*: *//"` && \
	BUILDARCH=`rpm --showrc | grep "^buildarch" | sed "s/buildarch[  ]*: *//"` && \

	cp RealTimeBattle.tar.gz $SOURCEDIR && \
	sed -e "s/#VERSION#/$(VERSION)/" -e "s/#RELEASE#/$(RELEASE)/" RealTimeBattle.spec > "$SPECDIR/RealTimeBattle-$(VERSION)-$(RELEASE).spec" && \
	rpm -ba "$SPECDIR/RealTimeBattle-$(VERSION)-$(RELEASE).spec" && \
	cp "$RPMDIR/$BUILDARCH/RealTimeBattle-$(VERSION)-$(RELEASE).$BUILDARCH.rpm" . && \
	cp "$SRCRPMDIR/RealTimeBattle-$(VERSION)-$(RELEASE).src.rpm" . 
