test : src/Image/loader.c src/Preprocess/grayscale.c src/Preprocess/binary.c
	gcc src/Image/loader.c src/Preprocess/grayscale.c src/Preprocess/binary.c -o test -Iincludes `pkg-config --cflags --libs sdl2 SDL2_image`


