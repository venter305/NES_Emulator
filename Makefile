_OBJ=$(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.cpp,%.o,$(wildcard GraphicsEngine/*.cpp))  $(patsubst %.cpp,%.o,$(wildcard GraphicsEngine/GUI/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard Mappers/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard SoundManager/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard Input/*.cpp))
OBJ=$(patsubst %,./obj/%,$(_OBJ))

IPATHS=-I/usr/include/freetype2
FLAGS= -lpulse-simple -lpulse

emu: $(OBJ)
	g++ $(OBJ) -o emu -lglut -lGL -lGLU -pthread -lglfw -lGLEW -lfreetype $(IPATHS) $(FLAGS)

obj/SoundManager/soundManager.o: ./SoundManager/soundManager.cpp ./SoundManager/soundManager.h
	g++ -c -o ./obj/SoundManager/soundManager.o ./SoundManager/soundManager.cpp $(IPATHS) $(FLAGS)

obj/%.o: %.cpp %.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) -O

obj/%.o: %.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS)

obj/%.o: %.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS)


init:
	mkdir -p obj  obj/GraphicsEngine obj/GraphicsEngine/GUI obj/Mappers obj/SoundManager obj/Input
	mkdir -p SaveStates
	mkdir -p Saves/

clean:
	rm -f ./obj/*.o ./obj/GraphicsEngine/*.o ./obj/GraphicsEngine/GUI/*.o ./obj/Mappers/*.o ./obj/SoundManager/*.o ./obj/Input/*.o emu
