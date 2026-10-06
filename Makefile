SRC=$(wildcard src/*.cpp) $(wildcard src/*.c)
SRC+=$(wildcard src/gfx/*.cpp) $(wildcard src/gfx/*.c)
SRC+=$(wildcard src/common/*.cpp) $(wildcard src/common/*.c)
SRC+=$(wildcard src/game/*.cpp) $(wildcard src/game/*.c)
SRC+=$(wildcard src/game/enemies/*.cpp) $(wildcard src/game/enemies/*.c)

HEADER=$(wildcard src/*.hpp) $(wildcard src/*.h)
HEADER+=$(wildcard src/gfx/*.hpp) $(wildcard src/gfx/*.h)
HEADER+=$(wildcard src/common/*.hpp) $(wildcard src/common/*.h)
HEADER+=$(wildcard src/game/*.hpp) $(wildcard src/game/*.h)
HEADER+=$(wildcard src/game/enemies/*.hpp) $(wildcard src/game/enemies/*.h)

OBJ=$(SRC:%=%.o)
CPP=c++
BIN_NAME=squish
INCLUDE=-Iinclude -iquote src/common -iquote src/game/ -iquote src/gfx/
FLAGS=$(INCLUDE) -std=c++17 -O2
LD_FLAGS=-lglfw3

ifeq ($(OS), Windows_NT)
	LD_FLAGS+=-static-libgcc -static-libstdc++ -lopengl32 -lgdi32 -mwindows
else
	LD_FLAGS+=-lGL
endif

output: $(OBJ)
	$(CPP) $(OBJ) -o $(BIN_NAME) $(FLAGS) $(LD_FLAGS)

%.cpp.o: %.cpp $(HEADER)
	$(CPP) $(FLAGS) -c $< -o $@ 

%.c.o: %.c $(HEADER)
	$(CPP) $(FLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJ) $(BIN_NAME)

run: output
	./$(BIN_NAME)

test_scene:
	./$(BIN_NAME) --test

test: $(OBJ)
	@cd tests && make -j$(nproc)
