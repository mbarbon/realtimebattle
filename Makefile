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

includedirs = -I/usr/lib/glib/include -I/usr/X11R6/include
libdirs = -L/usr/lib -L/usr/X11R6/lib
libraries = -lgtk -lgdk -lglib -lXext -lX11 -lm

all: RobotArena $(ROBOTDIR)/empty

RobotArena: robot.o Vector2D.o gui.o robotarena.o
	g++ $(cxxoptions) -o robotarena robot.o Vector2D.o gui.o robotarena.o $(includedirs) $(libdirs) $(libraries)

robot.o: robot.cc robot.h Vector2D.h
	g++ $(cxxoptions) -c robot.cc

Vector2D.o: Vector2D.cc Vector2D.h
	g++ $(cxxoptions) -c Vector2D.cc

gui.o: gui.c gui.h
	gcc $(coptions) -c gui.c $(includedirs)

robotarena.o: robotarena.cc robot.h Vector2D.h
	g++ $(cxxoptions) -c robotarena.cc $(includedirs)

$(ROBOTDIR)/empty: $(ROBOTDIR)/empty.o
	cd $(ROBOTDIR); \
	gcc $(coptions) -o empty empty.o; \
	cd ..

empty.o: $(ROBOTDIR)/empty.c
	gcc $(coptions) -c -o empty.o empty.c

clean:
	rm -f *~ *.o core* $(ROBOTDIR)/*~ $(ROBOTDIR)/*.o $(ROBOTDIR)/core*
