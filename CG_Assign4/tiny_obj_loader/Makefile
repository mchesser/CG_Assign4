CC = g++
CFLAGS = -c -O

SRC_FILES = tiny_obj_loader.cc
OBJECTS = $(SRC_FILES:.cc=.o)

LIB_FILE = tiny_obj_loader.a

all: $(SRC_FILES) $(LIB_FILE)

$(LIB_FILE): $(OBJECTS)
	ar rvs $(LIB_FILE) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(LIB_FILE)
	rm -rf *.o

.PHONY: \
	all \
	clean
