CC = g++
CFLAGS = --std=c++11 -O3 -Iinclude -Llib
LDFLAGS =

SRC := $(wildcard src/*.cpp)
SRC += $(wildcard src/**/*.cpp)
OBJ := $(patsubst %.cpp, obj/%.o, $(notdir $(SRC)))
HEADERS := $(wildcard src/*.hpp)
HEADERS += $(wildcard src/**/*.hpp)
LIBS := $(wildcard lib/*.o)
TARGET = chip-8

#mingw
ifeq ($(OS),Windows_NT)
	#CFLAGS +=
	LDFLAGS += -lmingw32 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -limm32
#unix
else
	#CFLAGS += $(shell pkg-config --cflags glfw3)
	#LDFLAGS += $(shell pkg-config --static --libs glfw3)
	LDFLAGS += -L/usr/lib64 -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp
endif

all: $(TARGET)

clean:
	-rm -r obj
	-rm $(TARGET)*

run: $(TARGET)
	./$<

rebuild:
	make clean
	make all

libs:
	./vendor/build_libs.sh

.PHONY: all clean run rebuild libs

$(TARGET): $(OBJ) | lib/libglfw3.a $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

lib/libglfw3.a:
	./vendor/build_libs.sh

lib/*.o:
	./vendor/build_libs.sh

obj/%.o: src/%.cpp $(HEADERS) | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/**/%.cpp $(HEADERS) | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir $@
