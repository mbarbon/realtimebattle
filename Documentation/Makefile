
target: RealTimeBattle.html

all: RealTimeBattle.html RealTimeBattle.dvi RealTimeBattle.info

documents = RealTimeBattle.udo Introduction.udo Usage.udo Options.udo RobotConstruction.udo ArenaConstruction.udo

RealTimeBattle.html: $(documents)
	udo --html --verbose -o RealTimeBattle.html RealTimeBattle.udo

RealTimeBattle.latex: $(documents)
	udo --tex --verbose  -o RealTimeBattle.latex RealTimeBattle.udo

RealTimeBattle.dvi: RealTimeBattle.latex
	latex RealTimeBattle.latex 

RealTimeBattle.info: $(documents)
	udo --info  --verbose -o RealTimeBattle.texinfo RealTimeBattle.udo; \
	makeinfo RealTimeBattle.texinfo

clean:
	rm -f *~ *.{aux,log,toc,latex,dvi,html,texinfo,info,ulh,ult,uli,gif}
