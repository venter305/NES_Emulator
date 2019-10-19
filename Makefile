_OBJ=$(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.cpp,%.o,$(wildcard GUI/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard Mappers/*.cpp))
OBJ=$(patsubst %,./obj/%,$(_OBJ))

emu: $(OBJ)
	g++ $(OBJ) -o emu -lglut -lGL -lGLU -pthread -lglfw -lGLEW -lfreetype -I/usr/include/freetype2

obj/%.o: %.cpp %.h
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: %.cpp
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: %.h
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./GUI/%.cpp ./GUI/%.h
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./GUI/%.cpp
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./GUI/%.h
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./Mappers/%.cpp ./Mappers/%.h
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./Mappers/%.cpp
	g++ -c -o $@ $< -I /usr/include/freetype2

obj/%.o: ./Mappers%.h
	g++ -c -o $@ $< -I /usr/include/freetype2

init:
	mkdir -p obj obj/GUI obj/Mappers
	mkdir -p SaveStates
	mkdir -p Saves/

clean:
	rm -f ./obj/*.o ./obj/GUI/*.o ./obj/Mappers/*.o emu

