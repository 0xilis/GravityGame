
# Makefile by Snoolie K / 0xilis (me!). This is my second Makefile so apologies if it is not the best.

output: ./build/main.o ./build/seajson.o ./build/collision.o ./build/hey_modders.o
	@if [ -d "./build/out" ]; \
	then \
		clang ./build/main.o ./build/seajson.o ./build/collision.o ./build/controller.o ./build/zones.o ./build/sound.o ./build/resource_management.o ./build/hey_modders.o -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -I/usr/local/include/SDL2 -D_THREAD_SAFE -o ./build/out/Gravity; \
		mv ./build/out/Gravity ./Gravity; \
	else \
		echo "Oh my god, please create ./build/out directory before running make, you heartless bastard!"; \
		exit 1; \
	fi

./build/main.o: ./engine/main.c
	@if [ -d "./build" ]; \
	then \
		clang -c ./engine/main.c ./engine/resource_management.c ./engine/zones.c ./engine/sound.c ./engine/controller.c -Os -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -I/usr/local/include/SDL2 -D_THREAD_SAFE; \
		mv ./main.o ./build/main.o; \
		mv ./resource_management.o ./build/resource_management.o; \
		mv ./zones.o ./build/zones.o; \
		mv ./sound.o ./build/sound.o; \
		mv ./controller.o ./build/controller.o; \
	else \
		echo "Oh my god, please create ./build directory before running make, you heartless bastard!"; \
		exit 1; \
	fi

./build/seajson.o: ./engine/seajson.c
	@if [ -d "./build" ]; \
	then \
		clang -c ./engine/seajson.c -Os -o ./build/seajson.o; \
	else \
		echo "Oh my god, please create ./build directory before running make, you heartless bastard!"; \
		exit 1; \
	fi

./build/collision.o: ./engine/collision.c
	@if [ -d "./build" ]; \
	then \
		clang -c ./engine/collision.c -Os -ffast-math -o ./build/collision.o; \
	else \
		echo "Oh my god, please create ./build directory before running make, you heartless bastard!"; \
		exit 1; \
	fi

./build/hey_modders.o: ./engine/hey_modders.c
	@if [ -d "./build" ]; \
	then \
		clang -c ./engine/hey_modders.c -Os -ffast-math -o ./build/hey_modders.o; \
	else \
		echo "Oh my god, please create ./build directory before running make, you heartless bastard!"; \
		exit 1; \
	fi

clean:
	rm ./build/main.o
	rm ./build/controller.o
	rm ./build/collision.o
	rm ./build/resource_management.o
	rm ./build/seajson.o
	rm ./build/sound.o
	rm ./build/zones.o
	@echo "Remove ./build yourself, it's not that hard."