
ifndef PAPER_SIZE
PAPER_SIZE=a4
endif


target: RealTimeBattle.html note-files

all: target RealTimeBattle.dvi RealTimeBattle.info RealTimeBattle.txt note-files.html

html: RealTimeBattle.html note-files.html

note-files: ../BUGS ../FAQ ../INSTALL ../NEWS ../README ../TODO

note-files.html: BUGS.html FAQ.html INSTALL.html NEWS.html README.html TODO.html

RealTimeBattle.html: RealTimeBattle.sgml
	sgml2html --imagebuttons RealTimeBattle.sgml

RealTimeBattle.dvi: RealTimeBattle.sgml
	sgml2latex -p $(PAPER_SIZE) -o dvi RealTimeBattle.sgml

RealTimeBattle.ps: RealTimeBattle.sgml
	sgml2latex -p $(PAPER_SIZE) -o ps RealTimeBattle.sgml

RealTimeBattle.info: RealTimeBattle.sgml
	sgml2info RealTimeBattle.sgml

RealTimeBattle.txt: RealTimeBattle.sgml
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


NEWS.html: NEWS.sgml
	sgml2html --split=0 NEWS.sgml

../NEWS: NEWS.sgml
	sgml2txt NEWS.sgml &&\
	sed -e "s/.//g" NEWS.txt > ../NEWS


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


clean:
	rm -f *~ *.{aux,log,toc,tex,dvi,html,info,ps,txt}
