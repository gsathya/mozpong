CXX = clang
SDL = -lSDL2

CXXFLAGS = -Wall -c
LDFLAGS = $(SDL)
EXE = pong

all: $(EXE)

$(EXE): main.o
	$(CXX) $(LDFLAGS) $< -o $@

main.o: main.c
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
