ROBOTDIR = Robots

#debug-options
coptions = -g -Wall
cxxoptions = -g -Wall

#normal-options
#coptions = -Wall
#cxxoptions = -Wall

#final-options
#coptions = -O
#cxxoptions = -O

all: RobotArena $(ROBOTDIR)/empty

RobotArena: robot.o pos.o gui.o robotarena.o
	g++ $(cxxoptions) -o robotarena robot.o pos.o gui.o robotarena.o -I/usr/lib/glib/include -I/usr/X11R6/include -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -lglib -lXext -lX11 -lm

robot.o: robot.cc robot.h pos.h
	g++ $(cxxoptions) -c robot.cc

pos.o: pos.cc pos.h
	g++ $(cxxoptions) -c pos.cc

gui.o: gui.c gui.h
	gcc $(coptions) -c gui.c -I/usr/lib/glib/include -I/usr/X11R6/include

robotarena.o: robotarena.cc robot.h pos.h
	g++ $(cxxoptions) -c robotarena.cc -I/usr/lib/glib/include -I/usr/X11R6/include

$(ROBOTDIR)/empty: $(ROBOTDIR)/empty.o
	cd $(ROBOTDIR); \
	gcc $(coptions) -o empty empty.o; \
	cd ..

empty.o: $(ROBOTDIR)/empty.c
	gcc $(coptions) -c -o empty.o empty.c

clean:
	rm -f *~ *.o core* $(ROBOTDIR)/*~ $(ROBOTDIR)/*.o $(ROBOTDIR)/core*