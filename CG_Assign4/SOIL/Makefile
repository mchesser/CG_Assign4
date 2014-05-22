CC = gcc
CFLAGS = -c -O -DWIN32

SRC_FILES = image_DXT.c image_helper.c stb_image_aug.c SOIL.c
OBJECTS = $(SRC_FILES:.c=.o)

LIB_FILE = SOIL.a

all: $(SRC_FILES) $(LIB_FILE)

$(LIB_FILE): $(OBJECTS)
	ar rvs $(LIB_FILE) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(LIB_FILE)
	rm -rf *.o

.PHONY: \
	all \
	clean
