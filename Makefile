.PHONY: all compile run png clean

CXX ?= g++
CXXFLAGS ?= -O2 -fopenmp

# Apple clang does not accept -fopenmp directly; it needs -Xpreprocessor
# plus the Homebrew libomp paths. Detect Darwin and switch automatically so
# `make` works on both the Linux box and the Mac.
ifeq ($(shell uname -s),Darwin)
  BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
  ifneq ($(wildcard $(BREW_PREFIX)/opt/libomp/include/omp.h),)
    CXXFLAGS := -O2 -Xpreprocessor -fopenmp \
                -I$(BREW_PREFIX)/opt/libomp/include \
                -L$(BREW_PREFIX)/opt/libomp/lib -lomp
  else
    $(warning libomp not found - run: brew install libomp)
  endif
endif

all: run

compile: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

run: compile
	./main > output.ppm
	@command -v magick >/dev/null 2>&1 && magick output.ppm output.png || \
	 echo "note: ImageMagick (magick) not found; output.ppm written"

clean:
	rm -f main output.ppm output.png
