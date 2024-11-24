EXEC = NeuralNetwork

CFLAGS = -Wall -Wextra -std=c99 -IImage `pkg-config --cflags sdl2 SDL2_image` -g
LDFLAGS = `pkg-config --libs sdl2 SDL2_image` -lm -fsanitize=address

SRC = $(wildcard ../Image/*.c  *.c)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

format:
	clang-format -i $(SRC) $(wildcard ../Image.h *.h)

clean:
	rm -f $(OBJ) $(EXEC)

clean-all: clean
	rm -f $(EXEC)