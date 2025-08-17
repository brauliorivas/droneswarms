CC := gcc
CFLAGS := -Wall
NAME := droneswarms
TARGET := $(NAME)
HEADER_FILES := $(wildcard *.h)
SRC_FILES := $(wildcard *.c)

OBJ_FILES := $(SRC_FILES:.c=.o)


$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES)

%.o: %.c $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g
debug: $(TARGET)

clean:
	rm -f $(OBJ_FILES) $(TARGET) 
