.PHONY: all compile run png

all: run

compile: main.cpp
	nvc++ -acc -o main main.cpp

run: compile
	./main > output.ppm
	magick output.ppm output.png