
ifndef PAPER_SIZE
PAPER_SIZE=a4
endif


target: RealTimeBattle.html

all: RealTimeBattle.html RealTimeBattle.dvi RealTimeBattle.info RealTimeBattle.txt

RealTimeBattle.html: RealTimeBattle.sgml
	sgml2html --imagebuttons RealTimeBattle.sgml

RealTimeBattle.dvi: RealTimeBattle.sgml
	sgml2latex -p $(PAPER_SIZE) -o dvi RealTimeBattle.sgml

RealTimeBattle.info: RealTimeBattle.sgml
	sgml2info RealTimeBattle.sgml

RealTimeBattle.txt: RealTimeBattle.sgml
	sgml2txt RealTimeBattle.sgml

clean:
	rm -f *~ *.{aux,log,toc,tex,dvi,html,info}
