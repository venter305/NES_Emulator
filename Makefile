_OBJ=$(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.cpp,%.o,$(wildcard GraphicsEngine/*.cpp))  $(patsubst %.cpp,%.o,$(wildcard GraphicsEngine/GUI/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard Mappers/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard SoundManager/*.cpp)) 
OBJ=$(patsubst %,./obj/%,$(_OBJ))

IPATHS=-I/usr/include/freetype2
FLAGS= -lpulse-simple -lpulse

emu: $(OBJ)
	g++ $(OBJ) -o emu -lglut -lGL -lGLU -pthread -lglfw -lGLEW -lfreetype $(IPATHS) $(FLAGS)

obj/%.o: %.cpp %.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: %.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: %.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/%.cpp ./GraphicsEngine/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/%.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/GUI/%.cpp ./GraphicsEngine/GUI/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/GUI/%.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./GraphicsEngine/GUI/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./Mappers/%.cpp ./Mappers/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./Mappers/%.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./Mappers/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./SoundManager/%.cpp ./SoundManager/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./SoundManager/%.cpp
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

obj/%.o: ./SoundManager/%.h
	g++ -c -o $@ $< $(IPATHS) $(FLAGS) 

init:
	mkdir -p obj  obj/GraphicsEngine obj/GraphicsEngine/GUI obj/Mappers obj/SoundManager
	mkdir -p SaveStates
	mkdir -p Saves/

clean:
	rm -f ./obj/*.o ./obj/GraphicsEngine/*.o ./obj/GraphicsEngine/GUI/*.o ./obj/Mappers/*.o ./obj/SoundManager/*.o emu

