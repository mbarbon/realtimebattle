all: rtbme

rtbme: AllGadgets.o Arena.o Gadget.o GadgetSet.o GadgetDef.o ShotGadget.o WeaponGadget.o WallGadget.o f_read.o Value.o Variable.o main.o MainWindow.o
	g++ -Wall -g *.o -o rtbme `gtk-config --cflags` \
	`gtk-config --libs`


main.o: main.cpp
	g++ -c -o main.o main.cc `gtk-config --cflags`

MainWindow.o: MainWindow.cpp
	g++ -c -o MainWindow.o MainWindow.cpp `gtk-config --cflags`

AllGadgets.o: AllGadgets.cc
	g++ -c -o AllGadgets.o AllGadgets.cc `gtk-config --cflags`

Arena.o: Arena.cc
	g++ -c -o Arena.o Arena.cc  `gtk-config --cflags`

Gadget.o: Gadget.cc
	g++ -c -o Gadget.o Gadget.cc `gtk-config --cflags`

GadgetSet.o: GadgetSet.cc
	g++ -c -o GadgetSet.o GadgetSet.cc `gtk-config --cflags`

GadgetDef.o: GadgetDefinition.cc
	g++ -c -o GadgetDef.o GadgetDefinition.cc `gtk-config --cflags`

ShotGadget.o: ShotGadget.cc
	g++ -c -o ShotGadget.o ShotGadget.cc `gtk-config --cflags`

WeaponGadget.o: WeaponGadget.cc
	g++ -c -o WeaponGadget.o WeaponGadget.cc `gtk-config --cflags`

WallGadget.o: WallGadget.cc
	g++ -c -o WallGadget.o WallGadget.cc `gtk-config --cflags`

f_read.o: f_read.cc
	g++ -c -o f_read.o f_read.cc `gtk-config --cflags`

Value.o: Value.cc
	g++ -c -o Value.o Value.cc `gtk-config --cflags`

Variable.o: Variable.cc
	g++ -c -o Variable.o Variable.cc `gtk-config --cflags`



clear:
	rm -f *.o *~ rtbme
