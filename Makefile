ROBOTDIR = Robots

#debug-options
#coptions = -g -Wall
#cxxoptions = -g -Wall

#normal-options
coptions = -Wall
cxxoptions = -Wall

#final-options
#coptions = -O
#cxxoptions = -O

gtkincludedirs = -I/usr/lib/glib/include -I/usr/X11R6/include
libdirs = -L/usr/lib -L/usr/X11R6/lib
libraries = -lgtk -lgdk -lglib -lXext -lX11 -lm

objects = robot.o Vector2D.o gui.o RealTimeBattle.o Arena.o Error.o Shape.o MovingObject.o Extras.o

all: RealTimeBattle $(ROBOTDIR)/empty

RealTimeBattle: $(objects)
	g++ $(cxxoptions) -o RealTimeBattle $(objects) $(gtkincludedirs) $(libdirs) $(libraries)

robot.o: robot.cc robot.h Vector2D.h
	g++ $(cxxoptions) -c robot.cc

Vector2D.o: Vector2D.cc Vector2D.h
	g++ $(cxxoptions) -c Vector2D.cc

gui.o: gui.cc gui.h
	g++ $(coptions) -c gui.cc $(gtkincludedirs)

RealTimeBattle.o: RealTimeBattle.cc Vector2D.h Arena.h messagetypes.h gui.h
	g++ $(cxxoptions) -c RealTimeBattle.cc $(gtkincludedirs)

Arena.o: Arena.cc Arena.h messagetypes.h Vector2D.h
	g++ $(cxxoptions) -c Arena.cc $(gtkincludedirs)

Error.o: Error.cc Arena.h messagetypes.h Vector2D.h
	g++ $(cxxoptions) -c Error.cc $(gtkincludedirs)

MovingObject.o: MovingObject.cc Arena.h messagetypes.h Vector2D.h
	g++ $(cxxoptions) -c MovingObject.cc $(gtkincludedirs)

Shape.o: Shape.cc Arena.h messagetypes.h Vector2D.h
	g++ $(cxxoptions) -c Shape.cc $(gtkincludedirs)

Extras.o: Extras.cc Arena.h messagetypes.h Vector2D.h
	g++ $(cxxoptions) -c Extras.cc $(gtkincludedirs)


$(ROBOTDIR)/empty: $(ROBOTDIR)/empty.o
	cd $(ROBOTDIR); \
	gcc $(coptions) -o empty empty.o; \
	cd ..

$(ROBOTDIR)/empty.o: $(ROBOTDIR)/empty.c
	cd $(ROBOTDIR); \
	gcc $(coptions) -c -o empty.o empty.c; \
	cd ..

clean:
	rm -f *~ *.o core* $(ROBOTDIR)/*~ $(ROBOTDIR)/*.o $(ROBOTDIR)/core*
