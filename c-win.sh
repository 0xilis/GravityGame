# https://stackoverflow.com/questions/32468761/how-can-i-link-with-gcc-and-sdl-image-library - linking SDL2_image
clang -o Gravity.exe -Wall main.c sound.c zones.c -O2 -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -I/usr/local/include/SDL2 -D_THREAD_SAFE
