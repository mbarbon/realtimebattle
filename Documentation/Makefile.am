docsdir = $(DOCS_DIR)/Documentation
docs_DATA = \
   RealTimeBattle-1.html RealTimeBattle-2.html RealTimeBattle-3.html \
   RealTimeBattle-4.html RealTimeBattle-5.html RealTimeBattle-6.html \
	RealTimeBattle.html next.gif prev.gif toc.gif RealTimeBattle.dvi


rtbinfodir = $(infodir)
rtbinfo_DATA = RealTimeBattle.info

EXTRA_DIST = $(docs_DATA)  $(rtbinfo_DATA)

install-data-local:
	gzip -f $(rtbinfodir)/RealTimeBattle.info; \
   if test "x$(INSTALL_INFO)" != "xno"; then \
		$(INSTALL_INFO) \
		--entry "* RealTimeBattle: (RealTimeBattle).             A robot programming game" \
		--info-dir $(rtbinfodir) $(rtbinfodir)/RealTimeBattle.info.gz; \
	fi || echo -e "\n\n** Info installation failed, please add the entry to dir by hand **\n\n"

uninstall-local:
	if test "x$(INSTALL_INFO)" != "xno"; then \
		$(INSTALL_INFO) --delete \
		--info-dir $(rtbinfodir) $(rtbinfodir)/RealTimeBattle.info.gz; \
	fi || echo -e "\n\n** Info uninstallation failed, please remove the entry from dir by hand **\n\n"; \
	rm -f $(rtbinfodir)/RealTimeBattle.info.gz; \
	rmdir -p $(DOCS_DIR)/Documentation || true


PAPER_SIZE = a4

docs: RealTimeBattle.html RealTimeBattle.dvi RealTimeBattle.info RealTimeBattle.txt \
		note-files note-files-html

html_docs: RealTimeBattle.html note-files-html

note-files: ../BUGS ../FAQ ../INSTALL ../ChangeLog ../README ../TODO

note-files-html: BUGS.html FAQ.html INSTALL.html ChangeLog.html README.html TODO.html



# Targets to generate the files if the sgml source exists, otherwise nothing happens 

%.html: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2html --split=0 -c latin $<"; \
		sgml2html --split=0 -c latin $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

RealTimeBattle.html: RealTimeBattle.sgml
	@if [ -f $< ]; then \
		echo "sgml2html --imagebuttons -D table=real -c latin $<"; \
		sgml2html --imagebuttons -D table=real -c latin $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.dvi: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2latex  -p $(PAPER_SIZE) -D table=real -c latin -v -o dvi $<"; \
		sgml2latex  -p $(PAPER_SIZE) -D table=real -c latin -v -o dvi $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.ps: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2latex  -p $(PAPER_SIZE) -D table=real -c latin -v -o ps $<"; \
		sgml2latex  -p $(PAPER_SIZE)  -D table=real -c latin -v -o ps $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.info: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2info  -D table=char -c latin $<"; \
		sgml2info  -D table=char -c latin $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.rtf: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2rtf -D table=real -c latin $<"; \
		sgml2rtf  -D table=real -c latin $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.lyx: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2lyx -D table=real -c latin $<"; \
		sgml2lyx  -D table=real -c latin $<; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

%.txt: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2txt --filter -D table=char -c latin $<"; \
		sgml2txt --filter -D table=char -c latin $<; \
#		echo "lynx -dump -nolist $< > $@"; \
#		lynx -dump -nolist $< > $@; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

../%: %.sgml
	@if [ -f $< ]; then \
		echo "sgml2txt --filter -D table=char -c latin $<"; \
		sgml2txt --filter -D table=char -c latin $<; \
#		echo "lynx -dump -nolist $< > $@"; \
#		lynx -dump -nolist $< > $@; \
		echo "mv $(subst .sgml,.txt,$<) $@"; \
		mv $(subst .sgml,.txt,$<) $@; \
	elif [ ! -f $@ ]; then \
		echo "Error: $@ is missing and cannot be generated"; \
	fi

#Just to allow for non-existing .sgml-file
%.sgml: 
	@true


