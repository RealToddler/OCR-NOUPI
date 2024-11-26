# Compiler and optiogs
CC = gcc
CFLAGS = -Wall -I$(SRC_DIR) -Wextra -g `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LIBS = `pkg-config --libs gtk+-3.0 sdl2 SDL2_image`

# Source and Build directories
SRC_DIR = src
BUILD_DIR = build

# Find all oc files in src and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.c')

# Object files will be place in build/ directory
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: ocr

# Link the program, include library flags
ocr: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	
# Rule to compile c files to o files, ensuring the build directory structure exists $(BUTLD_DIR)/%.0: $(SRC_DIR)/%.c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) # Create directory for object file if not exists
	$(CC) $(CFLAGS) -c $< -o $@
	
# Clean the build directory and program
clean:
	rm -rf $(BUILD_DIR) ocr

# Force recompilation
force: clean all

# Declare phony targets to avoid filename conflicts
.PHONY: all clean force
