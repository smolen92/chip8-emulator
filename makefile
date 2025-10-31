CXX = g++

CXXFLAGS = -Wall -g -lSDL3

chip8-emu.out: ./obj/main.o ./obj/chip8.o
	$(CXX) $(CXXFLAGS) -o chip8-emu.out ./obj/main.o ./obj/chip8.o

./obj/main.o: ./src/main.cpp ./obj/chip8.o
	$(CXX) $(CXXFLAGS) -c ./src/main.cpp -o ./obj/main.o

./obj/chip8.o: ./src/chip8.h ./src/chip8.cpp 
	$(CXX) $(CXXFLAGS) -c ./src/chip8.cpp -o ./obj/chip8.o

.PHONY: clean 

clean:
	rm -f ./obj/*.o
