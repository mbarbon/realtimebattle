all: main

main: AllGadgets.o Arena.o Gadget.o GadgetSet.o GadgetDef.o ShotGadget.o WeaponGadget.o WallGadget.o f_read.o Value.o Variable.o main.o
	g++ -o main *.o

main.o: main.cc
	g++ -c -o main.o main.cc

AllGadgets.o: AllGadgets.cc
	g++ -c -o AllGadgets.o AllGadgets.cc

Arena.o: Arena.cc
	g++ -c -o Arena.o Arena.cc

Gadget.o: Gadget.cc
	g++ -c -o Gadget.o Gadget.cc

GadgetSet.o: GadgetSet.cc
	g++ -c -o GadgetSet.o GadgetSet.cc

GadgetDef.o: GadgetDefinition.cc
	g++ -c -o GadgetDef.o GadgetDefinition.cc

ShotGadget.o: ShotGadget.cc
	g++ -c -o ShotGadget.o ShotGadget.cc

WeaponGadget.o: WeaponGadget.cc
	g++ -c -o WeaponGadget.o WeaponGadget.cc

WallGadget.o: WallGadget.cc
	g++ -c -o WallGadget.o WallGadget.cc

f_read.o: f_read.cc
	g++ -c -o f_read.o f_read.cc	

Value.o: Value.cc
	g++ -c -o Value.o Value.cc

Variable.o: Variable.cc
	g++ -c -o Variable.o Variable.cc



