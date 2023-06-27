#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

int sfs (int x) { //scale for screen
}
int main(void) {
	//https://www.youtube.com/watch?v=yFLa3ln16w0
	//https://stackoverflow.com/questions/52895444/c-image-loading-with-sdl2-and-sdl-image
	//https://www.youtube.com/watch?v=3VXtTH0AH3Q
	//https://stackoverflow.com/questions/71179555/sdl2-window-not-rendering
	//https://www.youtube.com/watch?v=XS3E7Q_5TMg

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
		printf("error with SDL: %s\n",SDL_GetError());
		return 1;
	}
	printf("Starting game...\n");
	SDL_Window* win = SDL_CreateWindow("Gravity",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1000, 1000, 0);
	if (!win) {
		printf("error creating window: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	//wait a few seconds
	//SDL_Delay(5000);

	//clean up res before exit
	//SDL_DestroyWindow(win);
	//SDL_Quit();

	//create renderer, set up graphics hardware
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
	if (!rend) {
		printf("error creating renderer: %s\n",SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	/*SDL_Surface* surface = IMG_Load("resources/hello.png");
	if (!surface) {
		printf("error creating surface\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}*/

	//load img data into graphics hw mem
	SDL_Texture *tex = IMG_LoadTexture(rend, "resources/hello.png");  //SDL_CreateTextureFromSurface(rend, surface);
	//SDL_FreeSurface(surface);
	if (!tex) {
		printf("error create texture\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	printf("we got here!\n");

	//clear window
	SDL_RenderClear(rend);

	//draw img to window
	//SDL_RenderCopy(rend, tex, NULL, NULL);
	SDL_Rect rect = {100, 100, 100, 100};
	SDL_RenderCopy(rend, tex, NULL, &rect);
	SDL_RenderPresent(rend);
	printf("drew sprite\n");

	int running = 1;
	SDL_Event event;
	int isUp = 0;
	int isDown = 0;
	int isLeft = 0;
	int isRight = 0;
	while(running != 0) {
		/*while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			} else if (event.type == SDL_KEYDOWN) {
				int keyCode = event.key.keysym.sym;
				const char *key = SDL_GetKeyName(event.key.keysym.sym);
				if(strcmp(key,"C") == 0) {
					//SDL_RenderClear(rend);
					SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
					SDL_RenderClear(rend);
					SDL_RenderPresent(rend);
				} else if (keyCode == SDLK_UP) {
					SDL_RenderClear(rend);
					isUp += 1;
					rect.y -= isUp;
					SDL_RenderCopy(rend, tex, NULL, &rect);
					SDL_RenderPresent(rend);
				} else if (keyCode == SDLK_DOWN) {
					SDL_RenderClear(rend);
					isDown += 1;
					rect.y += isDown;
					SDL_RenderCopy(rend, tex, NULL, &rect);
					SDL_RenderPresent(rend);
				} else if (keyCode == SDLK_LEFT) {
					SDL_RenderClear(rend);
					isLeft += 1;
					rect.x -= isLeft;
					SDL_RenderCopy(rend, tex, NULL, &rect);
					SDL_RenderPresent(rend);
				} else if (keyCode == SDLK_RIGHT) {
					SDL_RenderClear(rend);
					isRight += 1;
					rect.x += isRight;
					SDL_RenderCopy(rend, tex, NULL, &rect);
					SDL_RenderPresent(rend);
				}
			} else {
				isUp = 0;
				isDown = 0;
				isLeft = 0;
				isRight = 0;
				SDL_Delay(1);
			}
		}*/
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			running = 0;
		} else if (event.type == SDL_KEYDOWN) {
			int keyCode = event.key.keysym.sym;
			const char *key = SDL_GetKeyName(event.key.keysym.sym);
			if(strcmp(key,"C") == 0) {
				//SDL_RenderClear(rend);
				SDL_SetRenderDrawColor(rend, 100, 100, 100, 255);
				SDL_RenderClear(rend);
				SDL_RenderPresent(rend);
			} else if (keyCode == SDLK_UP) {
				SDL_RenderClear(rend);
				isUp += 1;
				rect.y -= isUp;
				SDL_RenderCopy(rend, tex, NULL, &rect);
				SDL_RenderPresent(rend);
			} else if (keyCode == SDLK_DOWN) {
				SDL_RenderClear(rend);
				isDown += 1;
				rect.y += isDown;
				SDL_RenderCopy(rend, tex, NULL, &rect);
				SDL_RenderPresent(rend);
			} else if (keyCode == SDLK_LEFT) {
				SDL_RenderClear(rend);
				isLeft += 1;
				rect.x -= isLeft;
				SDL_RenderCopy(rend, tex, NULL, &rect);
				SDL_RenderPresent(rend);
			} else if (keyCode == SDLK_RIGHT) {
				SDL_RenderClear(rend);
				isRight += 1;
				rect.x += isRight;
				SDL_RenderCopy(rend, tex, NULL, &rect);
				SDL_RenderPresent(rend);
			}
		} else {
			isUp = 0;
			isDown = 0;
			isLeft = 0;
			isRight = 0;
			SDL_Delay(1);
		}
		SDL_Delay(10); //so don't use 100% cpu
	}

	//wait a few sec
	//SDL_Delay(5000);
	printf("closing...\n");

	//clean up res before exit
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
