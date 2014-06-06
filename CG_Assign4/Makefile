# Makefile based on code from: http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles

CC = g++
CFLAGS = -c -g -Wall -DGLM_FORCE_RADIANS

# Determine the OS which is running (see: http://stackoverflow.com/questions/714100/os-detecting-makefile)
ifeq ($(OS),Windows_NT)
    OSFLAG = WIN32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OSFLAG = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        OSFLAG = __APPLE__
    endif
endif
export OSFLAG

SRC_FILES = Main.cpp Renderer.cpp Camera.cpp ModelData.cpp Shapes.cpp Object.cpp City.cpp Sun.cpp Skybox.cpp BuildingFactory.cpp
OBJECTS = $(SRC_FILES:.cpp=.o)
SOIL_LIB = SOIL2/SOIL2.a
TINY_OBJ_LIB = tiny_obj_loader/tiny_obj_loader.a

ifeq ($(OSFLAG), WIN32)
	CFLAGS += -DWIN32
	LIBS = -lopengl32 -lglut32 -lglew32 -lglu32
endif

ifeq ($(OSFLAG), __APPLE__)
	CFLAGS += -DMACOSX -I.
	LIBS = -framework Glut -framework OpenGL -framework CoreFoundation
endif

ifeq ($(OSFLAG), LINUX)
	LIBS = -lGL -lglut -lGLEW -lGLU
endif


BIN_FILE = assignment4

all: $(SRC_FILES) $(BIN_FILE)

$(BIN_FILE): $(OBJECTS) $(SOIL_LIB) $(TINY_OBJ_LIB)
	$(CC) -o $@ $(OBJECTS) $(SOIL_LIB) $(TINY_OBJ_LIB) $(LIBS)

$(SOIL_LIB):
	$(MAKE) -C SOIL2

$(TINY_OBJ_LIB):
	$(MAKE) -C tiny_obj_loader
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN_FILE)
	rm -rf *.o
	$(MAKE) -C SOIL2 clean
	$(MAKE) -C tiny_obj_loader clean

.PHONY: \
	all \
	clean \
	SOIL
