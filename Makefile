
define IncludePaths
GraphicsEngine
GraphicsEngine/GUI
GraphicsEngine/GUI/Layout
GraphicsEngine/Input
SoundManager
Mappers
endef

_OBJ=$(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(foreach dir,$(IncludePaths),$(patsubst %.cpp,%.o,$(wildcard $(dir)/*.cpp)))

OBJ=$(patsubst %,./obj/%,$(_OBJ))

INCLUDE=-I./eigen -I/usr/include/freetype2

LFLAGS=-lGLEW -lglfw -lGL -lfreetype -lpulse-simple -lpulse

CXXFLAGS= $(LFLAGS) $(INCLUDE) -O2

emu: $(OBJ)
	g++ $(OBJ) -o emu $(CXXFLAGS)


init:
	mkdir -p obj obj/dep $(foreach dir,$(IncludePaths),obj/$(dir))  $(foreach dir,$(IncludePaths),obj/dep/$(dir))
	mkdir -p SaveStates
	mkdir -p Saves

clean:
	rm -f ./gtest ./obj/*.o ./obj/dep/*.d $(foreach dir,$(IncludePaths),obj/$(dir)/*.o) $(foreach dir,$(IncludePaths),obj/dep/$(dir)/*.d)

./obj/dep/%.d: %.cpp
	@rm -f $@
	@g++ -MM $< $(CXXFLAGS) > $@.tmp
	@sed 's,\($(@F:.d=.o)\)[ :]*,$(patsubst obj/dep/%.d,./obj/%.o,$@) :,g' < $@.tmp > $@
	@sed -i '$$a \\tg++ -c $< -o $(patsubst obj/dep/%.d,./obj/%.o,$@) $(CXXFLAGS)' $@
	@rm -f $@.tmp

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),init)
-include $(patsubst %,./obj/dep/%,$(_OBJ:.o=.d))
endif
endif
