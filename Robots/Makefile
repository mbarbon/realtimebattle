CPPFLAGS =

all: empty fire

empty: empty.o
	$(CC) $(CPPFLAGS) $(CFLAGS) empty.o -o empty

fire: fire.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) fire.o -o fire

empty.o: empty.c 
fire.o: fire.cc ../messagetypes.h
