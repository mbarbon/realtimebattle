CPPFLAGS =

all: empty fire thomastest

empty: empty.o
	$(CC) $(CPPFLAGS) $(CFLAGS) empty.o -o empty

fire: fire.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) fire.o -o fire

thomastest: thomastest.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) thomastest.o -o thomastest

empty.o: empty.c 
fire.o: fire.cc ../messagetypes.h
thomastest.o: thomastest.cc ../messagetypes.h
