# Variables pour les fichiers sources
SRC = main.c Image/*.c Image/Detection/*.c Image/Preprocess/ColorTreatment/*.c \
      Image/Preprocess/NoiseReduction/*.c Image/Preprocess/Rotation/*.c

# Variable pour l'exécutable
EXEC = test

# Règle pour la compilation
$(EXEC): $(SRC)
	gcc $(SRC) -o $(EXEC) -IImage `pkg-config --cflags --libs sdl2 SDL2_image`

# Optionnel : Règle de nettoyage
clean:
	rm -f $(EXEC)