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
#include "resource_management.h"
#include "collision.h"
#include "controller.h"
#include <signal.h>

#include "defs.h"

#if __has_include(<Foundation/Foundation.h>)
#include "platform/objc/deltaTime.h"
#else
/* Not a platform with objc */
#endif

/* Colors */
#define WHITE_COLOR (SDL_Color){255, 255, 255, 255}
#define BLACK_COLOR (SDL_Color){0, 0, 0, 255}
#define PURPLE_COLOR (SDL_Color){128, 0, 128, 255}

/* Global variables */
SDL_Renderer* rend;
SDL_Rect background = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
int running = 1;
int isPaused = 0;
SDL_Rect pendingRect = {0, 0, 0, 0};
SDL_Rect pendingRectDisplay = {0, 0, 0, 0};
int awaitingNextRect = 0;
//double gravityDeltaTime = 0;
SDL_Rect *currentZoneDisplay;
SDL_Rect *currentZone;
int platformCount;
int controllerSuccessful;
SDL_GameController *controller;

/* Textures */
SDL_Texture *tex; /* Megumin test texture, player */
SDL_Texture *backgroundTex; /* Texture of the background */

PlayerPos playerPosition;

int coyote = 0;

int didHitMaxHeight = 0;
int isJumping = 0;
int jumpHeight = 0;
int moveProgress = 0;
int isGravityFlipped = 0;
unsigned int gravity = GRAVITY - 3;

__attribute__((always_inline)) static void screen_scrolling() {
    /* divide these into zones for performance (no need to scroll all platforms) */
    rect.x = (SCREEN_WIDTH/2 - PLAYER_WIDTH/2);
    if (playerPosition.x > CAMERA_MIN_X) {
        for (int i = 0; i < platformCount; i++) {
            currentZoneDisplay[i].x = currentZone[i].x - playerPosition.x;
        }
    } else {
        /* update rect for playerPosition.x */
        rect.x += (playerPosition.x - CAMERA_MIN_X);
    }
    
}

SDL_Texture *trick_sprites[8];

unsigned int player_animation_index;

SDL_Texture *load_sprite(const char *restrict sprite_path) {
    /* load img data into graphics hw mem */
    char *resource = find_resource(sprite_path);
    printf("resource (tex): %s\n",resource);
    SDL_Texture *tex = IMG_LoadTexture(rend, resource);
    if (!tex) {
        printf("error create texture\n");
        SDL_DestroyRenderer(rend);
        SDL_Quit();
        exit(1);
    }
    free(resource);
    return tex;
}

__attribute__((always_inline)) static void load_player_sprites(void) {
    /* Load all of trick's sprites */
    trick_sprites[0] = load_sprite("resources/trick_sprites/trick.png");
    trick_sprites[1] = load_sprite("resources/trick_sprites/trick-lookup.png");
    trick_sprites[2] = load_sprite("resources/trick_sprites/trick-lookleft.png");
    trick_sprites[3] = load_sprite("resources/trick_sprites/trick-lookleftup.png");
    trick_sprites[4] = load_sprite("resources/trick_sprites/trick-ud.png");
    trick_sprites[5] = load_sprite("resources/trick_sprites/trick-ud-lookup.png");
    trick_sprites[6] = load_sprite("resources/trick_sprites/trick-ud-lookleft.png");
    trick_sprites[7] = load_sprite("resources/trick_sprites/trick-ud-lookleftup.png");
    player_animation_index = 0;
}

void handle_events(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    /* This should probably be a switch case... */
    if (event.type == SDL_QUIT) {
        running = 0;
    } else if (event.type == SDL_KEYDOWN) {
        const char *key = SDL_GetKeyName(event.key.keysym.sym);
        if (strcmp(key,"Z") == 0) {
            if (!isPaused) {
                /* Flip Gravity */
                isGravityFlipped = !isGravityFlipped;
                gravity = GRAVITY - 3;
                /* Stop player from jumping higher */
                didHitMaxHeight = 1;
            }
        } else if (strcmp(key,"P") == 0) {
            if (isPaused) {
                /* Un-pause the game */
                isPaused = 0;
                resume_audio();
                if (awaitingNextRect) {
                    printf("Normal unpause, cancelled next rect.\n");
                    awaitingNextRect = 0;
                }
            } else {
                /* Pause the game */
                isPaused = 1;
                pause_audio();
            }
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            printf("Left click at (%d, %d)\n", event.button.x, event.button.y);
            if (awaitingNextRect) {
                awaitingNextRect = 0;
                pendingRect.w = event.button.x - pendingRect.x + playerPosition.x;
                /* If negative width, o shit, we messed up, let's patch this up */
                if (pendingRect.w < 0) {
                    pendingRect.x = pendingRect.x + pendingRect.w;
                    pendingRect.w *= -1;
                }
                pendingRect.h = event.button.y - pendingRect.y;
                /* If negative height, o shit, we messed up, let's patch this up */
                if (pendingRect.h < 0) {
                    pendingRect.y = pendingRect.y + pendingRect.h;
                    pendingRect.h *= -1;
                }
                platformCount += 1;
                currentZone = realloc(currentZone, sizeof(SDL_Rect) * platformCount);
                currentZoneDisplay = realloc(currentZoneDisplay, sizeof(SDL_Rect) * platformCount);
                currentZone[platformCount - 1] = pendingRect;
                currentZoneDisplay[platformCount - 1] = pendingRect;
                printf("Placed platform (%d,%d,%d,%d), and unpaused.\n", pendingRect.x, pendingRect.y, pendingRect.w, pendingRect.h);
                if (isPaused) {
                    resume_audio();
                    isPaused = 0;
                }
            } else {
                printf("Paused game. Please click the second point where you would like to place your platform.\n");
                awaitingNextRect = 1;
                pendingRect.x = event.button.x + playerPosition.x;
                pendingRect.y = event.button.y;
                if (!isPaused) {
                    pause_audio();
                    isPaused = 1;
                }
                pendingRectDisplay = pendingRect;
            }
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
            printf("Right click at (%d, %d)\n", event.button.x, event.button.y);
            SDL_Rect click = {event.button.x - 2, event.button.y - 2, 4, 4};
            int collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_ANY, click);
            if (collidingPlatform != -1) {
                printf("colliding platform!!!\n");
                /* Remove the colliding platform */
                /* I *could* realloc to take the less platform into account... but nah lol */
                for (int i = collidingPlatform; i < platformCount; i++) {
                    currentZone[i] = currentZone[i+1];
                    currentZoneDisplay[i] = currentZone[i+1];
                }
                platformCount--;
                printf("Removed platform.\n");
            }
        }
    } else if (event.type == SDL_CONTROLLERDEVICEADDED) {
        if (!controller) {
            controller = SDL_GameControllerOpen(event.cdevice.which);
        }
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
        if (controller && event.cdevice.which == SDL_JoystickInstanceID(
            SDL_GameControllerGetJoystick(controller))) {
            SDL_GameControllerClose(controller);
            controller = find_controller();
        }
    } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
        if (controller && event.cdevice.which == SDL_JoystickInstanceID(
            SDL_GameControllerGetJoystick(controller))) {
            if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
                if (!isPaused) {
                    /* Flip Gravity */
                    isGravityFlipped = !isGravityFlipped;
                    gravity = GRAVITY - 3;
                    /* Stop player from jumping higher */
                    didHitMaxHeight = 1;
                }
            } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                if (isPaused) {
                    /* Un-pause the game */
                    isPaused = 0;
                    resume_audio();
                    if (awaitingNextRect) {
                        printf("Normal unpause, cancelled next rect.\n");
                        awaitingNextRect = 0;
                    }
                } else {
                    /* Pause the game */
                    isPaused = 1;
                    pause_audio();
                }
            }
        }
    }
    /* Preview block place */
    if (awaitingNextRect) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        pendingRectDisplay.w = mouseX - pendingRect.x + playerPosition.x;
        /* If negative width, o shit, we messed up, let's patch this up */
        /*
         * Alright, so I have no idea how this works, but
         * for some reason pendingRectDisplay needs to have
         * the playerPosition.x subtracted from it, or else
         * we get some weird position errors.
         * I should note that this may not take into account if the player
         * moves while placing the rect since that would have made the playerPosition.x
         * that was used with the pendingRect.x set different, but we shouldn't
         * have to worry about that for now as placing a platform pauses the game.
        */
        if (pendingRectDisplay.w < 0) {
            pendingRectDisplay.x = pendingRect.x - playerPosition.x + pendingRectDisplay.w;
            pendingRectDisplay.w *= -1;
        } else {
            pendingRectDisplay.x = pendingRect.x - playerPosition.x;
        }
        pendingRectDisplay.h = mouseY - pendingRect.y;
        /* If negative height, o shit, we messed up, let's patch this up */
        if (pendingRectDisplay.h < 0) {
            pendingRectDisplay.y = pendingRect.y + pendingRectDisplay.h;
            pendingRectDisplay.h *= -1;
        } else {
            pendingRectDisplay.y = pendingRect.y;
        }
    }
    if (isPaused) {
        return;
    }

    /* Key presses */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    unsigned int pressingLeftButton = keys[SDL_SCANCODE_LEFT];
    unsigned int pressingRightButton = keys[SDL_SCANCODE_RIGHT];
/*
    if (pressingLeftButton && pressingRightButton) {
        pressingLeftButton = 0;
        pressingRightButton = 0;
    }
*/
    if (controller) {
        int axis_leftx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        if (axis_leftx < -CONTROLLER_DEADZONE) {
            pressingLeftButton = 1;
        } else if (axis_leftx > CONTROLLER_DEADZONE) {
            pressingRightButton = 1;
        }
    }
    if (pressingLeftButton) {
        player_animation_index = player_animation_index | 0x2;
        #ifdef LOCK_PLAYER_TO_CAMERA_MIN_X
        if ((rect.x + rect.w) > 0) {
        #endif
        /* Ensure we aren't colliding with anything before going left */
        if (!(get_collision_type() & COLLISION_IS_TOUCHING_RIGHT)) {
            int playerPosChange;
            if (moveProgress >= 27) {
                playerPosChange = 10;
            } else {
                moveProgress++;
                playerPosChange = (1 + (moveProgress / 3));
            }
            playerPosition.x -= playerPosChange;
            rect.x -= playerPosChange;
            /* Ensure that if we went inside of a platform, move us back */
            int theBadPlatform = colliding_platform(COLLISION_IS_TOUCHING_RIGHT,rect);
            if (theBadPlatform != -1) {
                playerPosition.x += (currentZoneDisplay[theBadPlatform].x + currentZoneDisplay[theBadPlatform].w) - rect.x;
            }
            rect.x += playerPosChange;
        }
        #ifdef LOCK_PLAYER_TO_CAMERA_MIN_X
        }
        #endif
    }
    if (pressingRightButton) {
        /* Discard PLAYER_SPRITE_LOOK_LEFT */
        player_animation_index = player_animation_index & 0xD;
        /* Ensure we aren't colliding with anything before going right */
        if (!(get_collision_type() & COLLISION_IS_TOUCHING_LEFT)) {
            int playerPosChange;
            if (moveProgress >= 27) {
                playerPosChange = 10;
            } else {
                moveProgress++;
                playerPosChange = (1 + (moveProgress / 3));
            }
            playerPosition.x += playerPosChange;
            rect.x += playerPosChange;
            /* Ensure that if we went inside of a platform, move us back */
            int theBadPlatform = colliding_platform(COLLISION_IS_TOUCHING_LEFT,rect);
            if (theBadPlatform != -1) {
                playerPosition.x -= rect.x - (currentZoneDisplay[theBadPlatform].x - PLAYER_WIDTH);
            }
            rect.x -= playerPosChange;
        }
    } else if (!pressingLeftButton) {
        moveProgress = 0;
    }
    unsigned int pressingJumpButton = keys[SDL_SCANCODE_UP];
    if (!pressingJumpButton) {
        pressingJumpButton = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
    }
    /* Set PLAYER_ANIM_GRAVITY_FLIP bit */
    player_animation_index &= 0xB;
    player_animation_index |= ((isGravityFlipped & 1) << 2);
    /* Discard jump animation index */
    player_animation_index = player_animation_index & 0xE;
    if (pressingJumpButton) {
        if (!didHitMaxHeight) {
            if (!isJumping) {
	        collision_type colType = get_collision_type();
                if ((isGravityFlipped && (colType & COLLISION_IS_TOUCHING_DOWN)) || (!isGravityFlipped && (colType & COLLISION_IS_TOUCHING_UP)) || coyote) {
                    coyote = 0; /* So you don't double-jump :P */
                    /* If player is both touching UP and DOWN, don't jump */
                    if (!((colType & COLLISION_IS_TOUCHING_UP) && (colType & COLLISION_IS_TOUCHING_DOWN))) {
                        isJumping = 1;
                        jump_sound();
                    }
                }
            }
            if (isJumping) {
                player_animation_index = player_animation_index | 1;
                if (!isGravityFlipped) {
                    /* cache the platform we're above */
                    /* This does respect the possibility of us jumping through two platforms :3 */
                    int platformToCheckIfClipped = above_platform_nointersect(rect);
                    rect.y -= (PLAYER_JUMP_VELOCITY_PLUS_GRAVITY - (0.3 * jumpHeight));
                    if (platformToCheckIfClipped != -1) {
                        /* check if we're below the same platform - if so, clip to it! */
                        if (below_platform_at_index(platformToCheckIfClipped, rect)) {
                            /* Damn it Jack, we moved too far! Go back! */
                            /* TODO: (BUG?) Gravity is applied after... */
                            rect.y = currentZoneDisplay[platformToCheckIfClipped].y + currentZoneDisplay[platformToCheckIfClipped].h;
                            isJumping = 0;
                            coyote = 0;
                        }
                    }
                } else {
                    /* cache the platform we're below */
                    /* This does respect the possibility of us jumping through two platforms :3 */
                    int platformToCheckIfClipped = below_platform_nointersect(rect);
                    rect.y += (PLAYER_JUMP_VELOCITY_PLUS_GRAVITY - (0.3 * jumpHeight));
                    if (platformToCheckIfClipped != -1) {
                        /* check if we're below the same platform - if so, clip to it! */
                        if (above_platform_at_index(platformToCheckIfClipped, rect)) {
                            /* Damn it Jack, we moved too far! Go back! */
                            /* TODO: (BUG?) Gravity is applied after... */
                            rect.y = currentZoneDisplay[platformToCheckIfClipped].y - PLAYER_HEIGHT;
                            isJumping = 0;
                            coyote = 0;
                        }
                    }
                }
                jumpHeight += (PLAYER_JUMP_VELOCITY - jumpHeight/4);
                if (jumpHeight >= PLAYER_JUMP_MAX_HEIGHT) {
                    didHitMaxHeight = 1;
                }
            }
        }
    } else {
        /* If player lets go of jump mid-jump, then make it so that that will be the height of their jump and even if they hold up again before they touch the ground they cannot jump again till they touch the ground */
        if (!coyote) {
            didHitMaxHeight = 1;
        }
    }
}

void update(void) {
    /* Screen scrolling */
    screen_scrolling();

    collision_type collidingPlatform;
    if (isGravityFlipped) {
        collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_DOWN, rect);
    } else {
        collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_UP, rect);
    }

    /* Check for collisions with platform */
    if (collidingPlatform == -1) {
        /* Apply gravity */
        if (isGravityFlipped) {
            if (rect.y > (-2 * SCREEN_HEIGHT)) {
                rect.y -= (gravity / 3);
            }
            collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_DOWN, rect);
        } else {
            if (rect.y < SCREEN_HEIGHT * 2) {
                rect.y += (gravity / 3);
            }
            collidingPlatform = colliding_platform(COLLISION_IS_TOUCHING_UP, rect);
        }
        if (gravity <= GRAVITY_MAX) {
            gravity++;
        }
    }

    /* this cannot be part of the else above bc we want to do this before render */
    if (collidingPlatform != -1) {
        /* If touching ground, not jumping currently!! */
        collision_type colType = get_collision_type_for_platform(collidingPlatform, rect);
        isJumping = 0;
        didHitMaxHeight = 0;
        jumpHeight = 0;
        gravity = GRAVITY - 3;
        coyote = COYOTE_MAX;
        /* if this is -1 then we KNOW col is COLLISION_IS_TOUCHING_DOWN instead */
        if (colType & COLLISION_IS_TOUCHING_UP) {
            /* clip to top of the platform */
            rect.y = currentZoneDisplay[collidingPlatform].y - PLAYER_HEIGHT;
        } else {
            /* clip to down of the platform */
            rect.y = currentZoneDisplay[collidingPlatform].y + currentZoneDisplay[collidingPlatform].h;
        }
    } else if (coyote != 0) {
        /* coyote timer */
        if (!isJumping) {
            coyote--;
        }
    }
}

void render(void) {
    /* Clear screen */
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, backgroundTex, NULL, &background);

    /* Draw platform */
    SDL_SetRenderDrawColor(rend, WHITE_COLOR.r, WHITE_COLOR.g, WHITE_COLOR.b, WHITE_COLOR.a);
    SDL_RenderFillRects(rend, currentZoneDisplay, platformCount);

    /* If awaiting platform, draw pending rect */
    if (awaitingNextRect) {
        SDL_SetRenderDrawColor(rend, PURPLE_COLOR.r, PURPLE_COLOR.g, PURPLE_COLOR.b, 100);
        SDL_RenderFillRect(rend, &pendingRectDisplay);
    }

    /* Gimme da image! */
    SDL_RenderCopy(rend, trick_sprites[player_animation_index], NULL, &rect);

    /* Render to screen */
    SDL_RenderPresent(rend);
}

int main(int argc,  char** argv) {
	/* find resources folder */
	if (argv) {
		char *executablePath = argv[0];
		if (executablePath) {
			resourcesPath = find_resource_path(executablePath);
		} else {
			fprintf(stderr,"GravityGame Error: argv[0] returned NULL\n");
			exit(1);
		}
	} else {
		fprintf(stderr,"GravityGame Error: No argv present??\n");
		exit(1);
	}
	printf("resourcePath: %s\n",resourcesPath);
	//signal(SIGSEGV, crash_handler); /* https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes */

	/* set playerPos to 0,0 */
	playerPosition.x = (640 - SCREEN_WIDTH) / 2;
	playerPosition.y = 0;

	/* load test zone into currentZone */
	char *resource = find_resource("level_dev.json");
	currentZone = load_level_as_full_zone_display(resource, &currentZoneDisplay);
	free(resource);
	resource = NULL;
	platformCount = platformsInCurrentZone();

        controllerSuccessful = 1; 
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER) != 0) {
            #ifndef FORCE_CONTROLLER
            /* failsafe: if GameController is the problem, try to init without */
            controllerSuccessful = 0;
            printf("error with initGameController: %s\n",SDL_GetError());
            if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
		printf("error with SDL: %s\n",SDL_GetError());
		return 1;
            }
            #else
            printf("error with SDL: %s\n",SDL_GetError());
            return 1;
            #endif
	}
        SDL_GameController *controller = find_controller();
	printf("Starting game...\n");
	SDL_Window* win = SDL_CreateWindow("Gravity",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!win) {
		printf("error creating window: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	/* create renderer, set up graphics hardware */
	/* Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC; */
        /*
         * According to https://nullprogram.com/blog/2023/01/08/
         * SDL2 already tries to create an accelerated renderer
         * and not specifying allows for a software renderer fallback
        */
        Uint32 render_flags = SDL_RENDERER_PRESENTVSYNC;
	rend = SDL_CreateRenderer(win, -1, render_flags);
	if (!rend) {
		printf("error creating renderer: %s\n",SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	/* Enable Blend/Opacity */
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

	/* Load player sprites */
	load_player_sprites();

	/* load bg img data into graphics hw mem */
	resource = find_resource("resources/pixil-frame-0.png");
	/* TODO: For some reason this only works with printf ??? if I remove the below line it no work ?? Look into latr */
	printf("resource (bgTex): %s\n",resource);
	backgroundTex = IMG_LoadTexture(rend, resource);
	if (!backgroundTex) {
		printf("error create bgTex\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	free(resource);
	resource = NULL;

	/* clear window */
	SDL_RenderClear(rend);

	/* init draw sprite */
	SDL_RenderCopy(rend, tex, NULL, &rect);
	SDL_RenderPresent(rend);

	/* play bg music */
	play_music(resourcesPath);

	SDL_SetRenderDrawColor(rend, WHITE_COLOR.r, WHITE_COLOR.g, WHITE_COLOR.b, WHITE_COLOR.a);

        /* initialize deltaTime */
        //init_delta_time();
        //gravityDeltaTime = get_delta_time();

	while (running) {
		/* Handle events */
                handle_events();
		
                if (!isPaused) {
		    /* Update game state */
		    update();
                }

		/* Render game state */
		render();

		//gravityDeltaTime = get_delta_time(); /* I hope we're supposed to calculate it here rather than after the delay... */

		//SDL_Delay(10); /* so don't use 100% cpu */
	}

	printf("closing...\n");

	/* clean up res before exit */
	free(resourcesPath);
	free(currentZone);
	free(currentZoneDisplay);
	cleanup_audio();
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}