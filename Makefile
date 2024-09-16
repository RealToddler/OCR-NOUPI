test : src/Image/* src/Preprocess/*
	gcc src/Image/* src/Preprocess/* -o test -Iincludes `pkg-config --cflags --libs sdl2 SDL2_image`


