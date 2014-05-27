CC = gcc
CFLAGS = -c -O -D$(OSFLAG)

SRC_FILES =  SOIL2.c etc1_utils.c image_DXT.c image_helper.c stb_image.c stb_image_write.c
OBJECTS = $(SRC_FILES:.c=.o)

LIB_FILE = SOIL2.a

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
