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
	if test "x$(INSTALL_INFO)" = "xyes"; then \
	install-info --entry "* RealTimeBattle: (RealTimeBattle).             A robot programming game" \
                --info-dir $(rtbinfodir) $(rtbinfodir)/RealTimeBattle.info.gz; \
	fi

uninstall-local:
	install-info --delete --info-dir $(rtbinfodir) $(rtbinfodir)/RealTimeBattle.info.gz; \
	rm $(rtbinfodir)/RealTimeBattle.info.gz

docs: RTB.html RTB.dvi RTB.info RTB.txt note-files note-files.html

html_docs: RTB.html note-files.html

note-files: ../BUGS ../FAQ ../INSTALL ../ChangeLog ../README ../TODO

note-files.html: BUGS.html FAQ.html INSTALL.html ChangeLog.html README.html TODO.html

RTB.html: RealTimeBattle.sgml
	sgml2html --imagebuttons RealTimeBattle.sgml

RTB.dvi: RealTimeBattle.sgml
	sgml2latex -p $(PAPER_SIZE) -o dvi RealTimeBattle.sgml

RTB.ps: RealTimeBattle.sgml
	sgml2latex -p $(PAPER_SIZE) -o ps RealTimeBattle.sgml

RTB.info: RealTimeBattle.sgml
	sgml2info RealTimeBattle.sgml

RTB.txt: RealTimeBattle.sgml
	sgml2txt RealTimeBattle.sgml


BUGS.html: BUGS.sgml
	sgml2html --split=0 BUGS.sgml

../BUGS: BUGS.sgml
	sgml2txt BUGS.sgml &&\
	sed -e "s/.//g" BUGS.txt > ../BUGS


FAQ.html: FAQ.sgml
	sgml2html --split=0 FAQ.sgml

../FAQ: FAQ.sgml
	sgml2txt FAQ.sgml &&\
	sed -e "s/.//g" FAQ.txt > ../FAQ


INSTALL.html: INSTALL.sgml
	sgml2html --split=0 INSTALL.sgml

../INSTALL: INSTALL.sgml
	sgml2txt INSTALL.sgml &&\
	sed -e "s/.//g" INSTALL.txt > ../INSTALL


ChangeLog.html: ChangeLog.sgml
	sgml2html --split=0 ChangeLog.sgml

../ChangeLog: ChangeLog.sgml
	sgml2txt ChangeLog.sgml &&\
	sed -e "s/.//g" ChangeLog.txt > ../ChangeLog


README.html: README.sgml
	sgml2html --split=0 README.sgml

../README: README.sgml
	sgml2txt README.sgml &&\
	sed -e "s/.//g" README.txt > ../README


TODO.html: TODO.sgml
	sgml2html --split=0 TODO.sgml

../TODO: TODO.sgml
	sgml2txt TODO.sgml &&\
	sed -e "s/.//g" TODO.txt > ../TODO
