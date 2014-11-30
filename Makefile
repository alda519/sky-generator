CXX=c++
CXXFLAGS=-pedantic -Wall -std=c++11 -g -O3 -MMD # -pg

SRCDIR=src
OBJFILES=$(SRCDIR)/main.o $(SRCDIR)/colorbuffer.o $(SRCDIR)/lodepng.o $(SRCDIR)/perlin.o $(SRCDIR)/raytracing.o

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
BIN=skygen
ANIBIN=animate
else
BIN=skygen.exe
ANIBIN=animate.exe
endif

.PHONY:all clean

all: $(BIN) $(ANIBIN)

$(BIN): $(OBJFILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(ANIBIN): $(SRCDIR)/animate.o $(SRCDIR)/colorbuffer.o $(SRCDIR)/lodepng.o $(SRCDIR)/perlin.o $(SRCDIR)/raytracing.o
	$(CXX) $(CXXFLAGS) -lSDL2 $^ -o $@


clean:
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.d $(BIN) $(ANIBIN)

-include $(OBJFILES:.o=.d)
