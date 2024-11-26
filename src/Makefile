EXEC = noupi

CFLAGS = -Wall -Wextra -std=c99 -IImage `pkg-config --cflags sdl2 SDL2_image gtk+-3.0` -g
LDFLAGS = `pkg-config --libs sdl2 SDL2_image gtk+-3.0` -lm -fsanitize=address

SRC = $(wildcard main.c Image/*.c Image/Detection/*.c Image/Preprocess/ColorTreatment/*.c \
      Image/Preprocess/NoiseReduction/*.c Image/Preprocess/Rotation/*.c Solver/*.c NeuralNetwork/*.c \
			UserInterface/*.c Files/*.c)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

format:
	clang-format -i $(SRC) $(wildcard Image/*.h Image/Detection/*.h Image/Preprocess/**/*.h Solver/*.h NeuralNetwork/*.h SysCommand/*.h Files/*.h)

clean:
	rm -f $(OBJ) $(EXEC)

clean-all: clean
	rm -f $(EXEC)
