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

PlayerPos playerPosition;

int check_platform_collision(void) {
    /* Check for collisions with platform */
    for (int i = 0; i < platformCount; i++) {
        if (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y && rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x <= currentZoneDisplay[i].x + currentZone[i].w && rect.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
	    return 1;
        }
        /* Left / Right collisions */
        /* Ensure we are on the same y level as the block */
        if ((rect.y >= (currentZoneDisplay[i].y - currentZoneDisplay[i].h)) && (rect.y + PLAYER_HEIGHT < currentZoneDisplay[i].y)) {
              /* Check for Left Collision */
              if (rect.x > currentZoneDisplay[i].x && rect.x < (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
              }
              return 1;  
        }
    }
    return 0;
}

int colliding_platform(collision_type collidingWithType) {
    /* Initialize our returnCollisionType variable */
    collision_type returnCollisionType = COLLISION_NOT_FOUND;

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Up / Down collisions */
        if (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y && rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x <= currentZoneDisplay[i].x + currentZone[i].w && rect.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
            if (rect.y < (currentZoneDisplay[i].y - (currentZoneDisplay[i].h / 2))) {
	        returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_UP);
            } else {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_DOWN);
            }
        }
        /* Left / Right collisions */
        /* Ensure we are on the same y level as the block */
        if ((rect.y > (currentZoneDisplay[i].y - currentZoneDisplay[i].h)) && (rect.y + PLAYER_HEIGHT <= currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
            /* Check for Left Collision */
            if (rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x < (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_LEFT);
            }
            /* Check for Right Collision */
            if (rect.x + PLAYER_WIDTH > currentZoneDisplay[i].x && rect.x <= (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_RIGHT);
            }
        }
        if (returnCollisionType & collidingWithType) {
            return i;
        }
    }
    return 0;
}

collision_type get_collision_type(void) {
    /* TODO:
     * This function has a bad bug regarding logic.
     * Basically, while it correctly can store both LEFT and RIGHT
     * (assuming the player is colliding with both directions)
     * However, the same is *NOT* currently true for UP and DOWN
     * So if player collides with both up and down at once, BAD!
     * Fix this latr.
     * NEVERMIND - Actually this is not as bad as I thought
     * Seems like it can only not correctly detect UP/DOWN
     * on one platform (and a player will never both be colliding
     * with the same platform in both directions at once.)
     * So hopefully I can forget about this bug.
    */

    /* Initialize our returnCollisionType variable */
    collision_type returnCollisionType = COLLISION_NOT_FOUND;

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Up / Down collisions */
        if (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y && rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x <= currentZoneDisplay[i].x + currentZone[i].w && rect.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
            if (rect.y < (currentZoneDisplay[i].y - (currentZoneDisplay[i].h / 2))) {
	        returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_UP);
            } else {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_DOWN);
            }
        }
        /* Left / Right collisions */
        /* Ensure we are on the same y level as the block */
        if ((rect.y > (currentZoneDisplay[i].y - currentZoneDisplay[i].h)) && (rect.y + PLAYER_HEIGHT <= currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
            /* Check for Left Collision */
            if (rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x < (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_LEFT);
            }
            /* Check for Right Collision */
            if (rect.x + PLAYER_WIDTH > currentZoneDisplay[i].x && rect.x <= (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_RIGHT);
            }
        }
    }
    return returnCollisionType;
}

int didHitMaxHeight = 0;
int isJumping = 0;
int jumpHeight = 0;
int moveProgress = 0;
int isGravityFlipped = 0;
int gravity = GRAVITY - 3;

__attribute__((always_inline)) static void screenScrolling() {
    /* divide these into zones for performance (no need to scroll all platforms) */
    for (int i = 0; i < platformCount; i++) {
        currentZoneDisplay[i].x = currentZone[i].x - playerPosition.x;
    }
}

void handle_events(void) {
    /* Key presses */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        /* Ensure we aren't colliding with anything before going left */
        if (!(get_collision_type() & COLLISION_IS_TOUCHING_RIGHT)) {
            if (moveProgress >= 20) {
                playerPosition.x -= 10;
            } else {
                moveProgress++;
                playerPosition.x -= (5 + (moveProgress / 4));
            }
            /* TODO: This is horrible. */
            screenScrolling();
            /* Ensure that if we went inside of a platform, move us back */
            if (get_collision_type() & COLLISION_IS_TOUCHING_RIGHT) {
                while (get_collision_type() & COLLISION_IS_TOUCHING_RIGHT) {
                    playerPosition.x += 1;
                    /* Screen-scroll just the misbehaving platform - thankfully we run screenScrolling() in update latr so this should work, but still, we screen scroll once, then do this, then screen scroll again - not exactly the fastest system, we should improve this tbh */
                    int theBadPlatform = colliding_platform(COLLISION_IS_TOUCHING_RIGHT);
                    if (theBadPlatform == 0) {
                        printf("FUCK\n");
                        break;
                    }
                    currentZoneDisplay[theBadPlatform].x -= 1;
                }
                /* Now, we are moved back to exactly before we start colliding with right */
                /* Move back 1 position forward to push us backward and move to our correct position */
                playerPosition.x--;
            }
        }
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        /* Ensure we aren't colliding with anything before going right */
        if (!(get_collision_type() & COLLISION_IS_TOUCHING_LEFT)) {
            if (moveProgress >= 20) {
                playerPosition.x += 10;
            } else {
                moveProgress++;
                playerPosition.x += 5 + (moveProgress / 4);
            }
            /* TODO: I am ASSUMING this does not work currently as get_collision_type needs us to screen scroll first before it correctly returns collision */
            screenScrolling();
            /* Ensure that if we went inside of a platform, move us back */
            if (get_collision_type() & COLLISION_IS_TOUCHING_LEFT) {
                while (get_collision_type() & COLLISION_IS_TOUCHING_LEFT) {
                    playerPosition.x -= 1;
                    /* Screen-scroll just the misbehaving platform - thankfully we run screenScrolling() in update latr so this should work, but still, we screen scroll once, then do this, then screen scroll again - not exactly the fastest system, we should improve this tbh */
                    int theBadPlatform = colliding_platform(COLLISION_IS_TOUCHING_LEFT);
                    currentZoneDisplay[theBadPlatform].x += 1;
                }
                /* Now, we are moved back to exactly before we start colliding with left */
                /* Move us 1 position forward to push us forward and move to our correct position */
                playerPosition.x++;
            }
        }
    }
    if (!keys[SDL_SCANCODE_LEFT] && !keys[SDL_SCANCODE_RIGHT]) {
        moveProgress = 0;
    }
    if (keys[SDL_SCANCODE_UP]) {
        if (check_platform_collision()) {
            if (!isJumping) {
                isJumping = 1;
                jump_sound();
            }
        }
        if (isJumping) {
            if (!didHitMaxHeight) {
                /* i really shoddadely tiredly added gravity to jumping, do better than this later when not tired and it is not 3:06 :P */
                if (!isGravityFlipped) {
                    rect.y -= (PLAYER_JUMP_VELOCITY - jumpHeight/4) + (GRAVITY - jumpHeight/20);
                } else {
                    rect.y += (PLAYER_JUMP_VELOCITY - jumpHeight/4) + (GRAVITY - jumpHeight/20);
                }
                jumpHeight += (PLAYER_JUMP_VELOCITY - jumpHeight/4);
                if (jumpHeight >= PLAYER_JUMP_MAX_HEIGHT) {
                    didHitMaxHeight = 1;
                }
            }
        }
    } else {
        /* If player lets go of jump mid-jump, then make it so that that will be the height of their jump and even if they hold up again before they touch the ground they cannot jump again till they touch the ground */
        didHitMaxHeight = 1;
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
    } else if (event.type == SDL_KEYDOWN) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"P") == 0) {
            /* Flip Gravity */
            isGravityFlipped = !isGravityFlipped;
            gravity = GRAVITY - 3;
            /* Sub 1 bc dump mitigation :P */
            rect.y--;
        }
    }
}

void update(void) {
    /* Screen scrolling */
    screenScrolling();

    /* Check for collisions with platform */
    if (!check_platform_collision()) {
        /* Apply gravity */
        if (isGravityFlipped) {
            if (rect.y > (-2 * SCREEN_HEIGHT)) {
                rect.y -= (gravity / 2);
            }
        } else {
            if (rect.y < SCREEN_HEIGHT * 2) {
                rect.y += (gravity / 2);
            }
        }
        if (gravity <= GRAVITY_MAX) {
            gravity++;
        }
    } else {
        gravity = GRAVITY - 3;
    }

    /* this cannot be part of the else above bc we want to do this before render */
    if (check_platform_collision()) {
        collision_type currentCollision = get_collision_type();
        if (currentCollision & COLLISION_IS_TOUCHING_UP) {
            /* clip to top of the platform */
            if (!isGravityFlipped) {
                rect.y = currentZoneDisplay[colliding_platform(COLLISION_IS_TOUCHING_UP)].y - PLAYER_HEIGHT;
            } else {
                rect.y = currentZoneDisplay[colliding_platform(COLLISION_IS_TOUCHING_UP)].y - PLAYER_HEIGHT - 1;
            }
        } else if (currentCollision & COLLISION_IS_TOUCHING_DOWN) {
            /* clip to down of the platform */
            int collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_DOWN);
            if (isGravityFlipped) {
                rect.y = currentZoneDisplay[collidingPlatform].y + currentZoneDisplay[collidingPlatform].h;
            } else {
                rect.y = currentZoneDisplay[collidingPlatform].y + currentZoneDisplay[collidingPlatform].h + 1;
            }
        }
    }

    /* Check for out of bounds */
    if (rect.y > SCREEN_HEIGHT) {
	/*printf("OUT OF BOUNDS\n");
        running = 0;*/
    }
}

void render(void) {
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
	currentZone = (SDL_Rect *)malloc(sizeof(SDL_Rect) * platforms_in_test_zone());
	load_test_zone(currentZone);
	currentZoneDisplay = (SDL_Rect *)malloc(sizeof(SDL_Rect) * platforms_in_test_zone());
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
	cleanup_audio();
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
