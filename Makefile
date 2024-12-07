CC = gcc
CFLAGS = -Wall -I$(SRC_DIR) -Wextra -fsanitize=address -g `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LIBS = `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm

SRC_DIR = src
BUILD_DIR = build

SRCS = $(shell find $(SRC_DIR) -name '*.c')

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: ocr

ocr: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) # Create directory for object file if not exists
	$(CC) $(CFLAGS) -c $< -o $@


clang-format:
	clang-format -i $(SRCS) $(shell find $(SRC_DIR) -name '*.h')


clean:
	rm -rf $(BUILD_DIR) ocr


force: clean all


.PHONY: all clean force
