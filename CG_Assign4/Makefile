# Makefile based on code from: http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles

CC = g++
CFLAGS = -c -O -Wall -DGLM_FORCE_RADIANS
# Windows = WIN32, MacOSX = __APPLE__, Linux = Linux
OSFLAG = __APPLE__

SRC_FILES = Main.cpp Renderer.cpp Camera.cpp ModelData.cpp Object.cpp Array2d.cpp City.cpp
OBJECTS = $(SRC_FILES:.cpp=.o)
SOIL_LIB = SOIL/SOIL.a
TINY_OBJ_LIB = tiny_obj_loader/tiny_obj_loader.a

ifeq ($(OSFLAG), WIN32)
CFLAGS += -DWIN32
LIBS = -lopengl32 -lglut32 -lglew32 -lglu32
endif

ifeq ($(OSFLAG), __APPLE__)
CFLAGS += -DMACOSX -I.
LIBS = -framework Glut -framework OpenGL -framework CoreFoundation
endif

ifeq ($(OSFLAG), Linux)
LIBS = -lGL -lglut -lGLEW -lGLU
endif


BIN_FILE = assignment4

all: $(SRC_FILES) $(BIN_FILE)

$(BIN_FILE): $(OBJECTS) $(SOIL_LIB) $(TINY_OBJ_LIB)
	$(CC) -o $@ $(OBJECTS) $(SOIL_LIB) $(TINY_OBJ_LIB) $(LIBS)

$(SOIL_LIB):
	$(MAKE) -C SOIL

$(TINY_OBJ_LIB):
	$(MAKE) -C tiny_obj_loader
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN_FILE)
	rm -rf *.o
	$(MAKE) -C SOIL clean
	$(MAKE) -C tiny_obj_loader

.PHONY: \
	all \
	clean \
	SOIL
