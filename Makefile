CC = g++
CFLAGS = -g -Wall -std=c++11
LIBS = `pkg-config opencv openalpr --cflags --libs`
OPATH = .object/

all: plate_generator

plate_generator:plate_generator.cpp $(OPATH)generate.o
	$(CC) $(CFLAGS) -o plate_generator plate_generator.cpp $(LIBS)

$(OPATH)generate.o:generate.cpp generate.hpp
	mkdir -p $(OPATH)
	$(CC) $(CFLAGS) -g -c generate.cpp -o $(OPATH)generate.o

plate_generator:plate_generator.cpp $(OPATH)plate.o
	$(CC) $(CFLAGS) -o plate_generator plate_generator.cpp $(LIBS)

$(OPATH)plate.o:plate.cpp plate.hpp
	mkdir -p $(OPATH)
	$(CC) $(CFLAGS) -g -c plate.cpp -o $(OPATH)plate.o
