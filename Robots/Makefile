ifndef CC
CC = gcc
CXX = g++
endif


targets:=$(shell ls -dF1 --color=never */ | grep -v CVS | sed -e "s/[/][/]*/.robot/")
clean_targets:=$(shell ls -dF1 --color=never */ | grep -v CVS | sed -e "s/[/][/]*/.clean/")

# if the above doesn't work, uncomment the lines below (and comment the lines above).
#
#targets = empty.robot seek_and_destroy.robot rotate_and_fire.robot
#clean_targets = empty.clean seek_and_destroy.clean rotate_and_fire.clean

target: $(targets)

%.robot: %
	cd $< && $(MAKE) && mv $@ ..

clean: $(clean_targets)
	rm -f *~ *.o core* *.robot

%.clean: %
	cd $< && $(MAKE) clean