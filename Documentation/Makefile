
all: RealTimeBattle.html RealTimeBattle.dvi RealTimeBattle.info

documents = RealTimeBattle.udo Introduction.udo Usage.udo Options.udo

RealTimeBattle.html: $(documents)
	udo --html -o RealTimeBattle.html RealTimeBattle.udo

RealTimeBattle.latex: $(documents)
	udo --tex -o RealTimeBattle.latex RealTimeBattle.udo

RealTimeBattle.dvi: RealTimeBattle.latex
	latex RealTimeBattle.latex 

RealTimeBattle.info: $(documents)
	udo --info -o RealTimeBattle.texinfo RealTimeBattle.udo; \
	makeinfo RealTimeBattle.texinfo

