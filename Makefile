test : src/Image/* src/Preprocess/* src/Detection/*
	gcc src/Image/* src/Preprocess/* src/Detection/* -o test -Iincludes `pkg-config --cflags --libs sdl2 SDL2_image`