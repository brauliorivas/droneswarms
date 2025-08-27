CC := gcc
CFLAGS := -Wall -Wextra
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

verbose: CFLAGS += -DERR_VERBOSE -DLOG_VERBOSE -g
verbose: $(TARGET)

err: CFLAGS += -DERR_VERBOSE -g
err: $(TARGET)

log: CFLAGS += -DLOG_VERBOSE -g
log: $(TARGET)

clean:
	rm -f $(OBJ_FILES) $(TARGET) 
