ifndef CC
CC = gcc
CXX = g++
endif


targets:=$(shell ls -d1 --color=never */ | grep -v CVS | sed -e "s/\//.robot/")
clean_targets:=$(shell ls -d1 --color=never */ | grep -v CVS | sed -e "s/\//.clean/")

target: $(targets)

%.robot: %
	cd $< && $(MAKE) && mv $@ ..

clean: $(clean_targets)
	rm -f *~ *.o core* *.robot

%.clean: %
	cd $< && $(MAKE) clean