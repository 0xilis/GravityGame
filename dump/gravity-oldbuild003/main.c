/*
 * Copyright (C) 2023 Snoolie K / 0xilis. All rights reserved.
 *
 * This document is the property of Snoolie K / 0xilis.
 * It is considered confidential and proprietary.
 *
 * This document may not be reproduced or transmitted in any form,
 * in whole or in part, without the express written permission of
 * Snoolie K / 0xilis.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "sound.h"
#include "zones.h"

#include "defs.h"

/* Colors */
#define WHITE_COLOR (SDL_Color){255, 255, 255, 255}
#define BLACK_COLOR (SDL_Color){0, 0, 0, 255}
#define PURPLE_COLOR (SDL_Color){128, 0, 128, 255}

/* Global variables */
SDL_Renderer* rend;
#define STARTING_PLATFORM_HEIGHT 200
SDL_Rect rect = {SCREEN_WIDTH/2 - PLAYER_WIDTH/2, SCREEN_HEIGHT - STARTING_PLATFORM_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};
SDL_Rect background = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
int running = 1;
SDL_Rect *currentZoneDisplay;
SDL_Rect *currentZone;
int platformCount;

/* Textures */
SDL_Texture *tex; /* Megumin test texture, player */
SDL_Texture *backgroundTex; /* Texture of the background */

typedef struct {
  int x;
  int y;
} PlayerPos;

PlayerPos playerPosition;

int check_platform_collision(void) {
    /* Check for collisions with platform */
    for (int i = 0; i < platformCount; i++) {
        if (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y && rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x <= currentZoneDisplay[i].x + currentZone[i].w) {
	    return 1;
        }
    }
    return 0;
}

int didHitMaxHeight = 0;
int isJumping = 0;
int jumpHeight = 0;

void handle_events(void) {
    /* Key presses */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        playerPosition.x -= 10;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        playerPosition.x += 10;
    }
    if (keys[SDL_SCANCODE_UP]) {
        if (check_platform_collision()) {
            isJumping = 1;
        }
        if (isJumping) {
            if (!didHitMaxHeight) {
                if (jumpHeight >= PLAYER_JUMP_HEIGHT_STAGE_2) {
                    rect.y -= PLAYER_JUMP_HEIGHT_STAGE_2_VELOCITY;
                    jumpHeight += PLAYER_JUMP_HEIGHT_STAGE_2_VELOCITY;
                } else {
                    rect.y -= PLAYER_JUMP_VELOCITY;
                    jumpHeight += PLAYER_JUMP_VELOCITY;
                }
                if (jumpHeight >= PLAYER_JUMP_MAX_HEIGHT) {
                    didHitMaxHeight = 1;
                }
            }
        }
    }

    if (check_platform_collision()) {
        /* If touching ground, not jumping currently!! */
        isJumping = 0;
        didHitMaxHeight = 0;
        jumpHeight = 0;
    }

    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        running = 0;
    }
}

void screenScrolling() {
    /* divide these into zones for performance (no need to scroll all platforms) */
    for (int i = 0; i < platformCount; i++) {
        currentZoneDisplay[i].x = currentZone[i].x - playerPosition.x;
    }
    //platform.x = platformOriginalPos.x - playerPosition.x;
}

void update() {
    /* Screen scrolling */
    screenScrolling();

    /* Check for collisions with platform */
    if (!check_platform_collision()) {
        /* Apply gravity */
        rect.y += 5;
    }

    /* Check for out of bounds */
    if (rect.y > SCREEN_HEIGHT) {
	printf("OUT OF BOUNDS\n");
        running = 0;
    }
}

void render() {
    /* Clear screen */
    SDL_SetRenderDrawColor(rend, BLACK_COLOR.r, BLACK_COLOR.g, BLACK_COLOR.b, BLACK_COLOR.a);
    SDL_RenderClear(rend);
    SDL_RenderFillRect(rend, &background);
    SDL_RenderCopy(rend, backgroundTex, NULL, &background);

    /* Draw platform */
    SDL_SetRenderDrawColor(rend, WHITE_COLOR.r, WHITE_COLOR.g, WHITE_COLOR.b, WHITE_COLOR.a);
    for (int i = 0; i < platformCount; i++) {
        SDL_RenderFillRect(rend, &currentZoneDisplay[i]);
    }

    /* Draw player */
    SDL_SetRenderDrawColor(rend, PURPLE_COLOR.r, PURPLE_COLOR.g, PURPLE_COLOR.b, PURPLE_COLOR.a);
    SDL_RenderFillRect(rend, &rect);
    /* Gimme da image! */
    SDL_RenderCopy(rend, tex, NULL, &rect);

    /* Render to screen */
    SDL_RenderPresent(rend);
}

int main(void) {
	/* set playerPos to 0,0 */
	playerPosition.x = 0;
	playerPosition.y = 0;

	/* load test zone into currentZone */
	currentZone = (SDL_Rect *)malloc(sizeof(SDL_Rect) * 2);
	load_test_zone(currentZone);
	currentZoneDisplay = (SDL_Rect *)malloc(sizeof(SDL_Rect) * 2);
	load_test_zone(currentZoneDisplay);
	platformCount = platformsInCurrentZone();

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
		printf("error with SDL: %s\n",SDL_GetError());
		return 1;
	}
	printf("Starting game...\n");
	SDL_Window* win = SDL_CreateWindow("Gravity",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!win) {
		printf("error creating window: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	/* create renderer, set up graphics hardware */
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	rend = SDL_CreateRenderer(win, -1, render_flags);
	if (!rend) {
		printf("error creating renderer: %s\n",SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	/* load img data into graphics hw mem */
	tex = IMG_LoadTexture(rend, "resources/hello.png");
	if (!tex) {
		printf("error create texture\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	/* load bg img data into graphics hw mem */
	backgroundTex = IMG_LoadTexture(rend, "resources/bgimage.jpeg");
	if (!backgroundTex) {
		printf("error create bgTex\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	/* clear window */
	SDL_RenderClear(rend);

	/* init draw sprite */
	SDL_RenderCopy(rend, tex, NULL, &rect);
	SDL_RenderPresent(rend);

	/* play bg music */
	play_music();

	while (running) {
		/* Handle events */
                handle_events();
		
		/* Update game state */
		update();

		/* Render game state */
		render();

		SDL_Delay(10); /* so don't use 100% cpu */
	}

	printf("closing...\n");

	/* clean up res before exit */
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
