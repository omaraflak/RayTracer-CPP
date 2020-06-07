all: main.o vect3.o
	g++ main.o vect3.o -o main
	./main

main.o: main.cpp
	g++ -c main.cpp

vect3.o: vect3.h vect3.cpp
	g++ -c vect3.cpp

png:
	pnmtopng out.ppm > out.png
