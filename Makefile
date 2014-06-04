CXX = clang
SDL = -lSDL2

CXXFLAGS = -Wall -c
LDFLAGS = $(SDL)
EXE = pong

all: $(EXE)

$(EXE): pong.o
	$(CXX) $(LDFLAGS) $< -o $@

main.o: pong.c
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
