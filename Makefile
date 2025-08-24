.PHONY: all compile run png

TARGET ?= main
SOURCE ?= main.cpp

all: run png

run: compile
	./main > output.ppm

compile: main.cpp
	g++ -O3 -fopenmp -o $(TARGET) $(SOURCE)

png: output.ppm
	magick output.ppm output.png

clean:
	rm -f main output.ppm output.png
