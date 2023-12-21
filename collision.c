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
#include "collision.h"

#include "defs.h"

/* Initialize rect */
SDL_Rect rect = {SCREEN_WIDTH/2 - PLAYER_WIDTH/2, SCREEN_HEIGHT - STARTING_PLATFORM_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};

int check_platform_collision(void) {
    /* Check for collisions with platform */
    for (int i = 0; i < platformCount; i++) {
        if (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y && rect.x + PLAYER_WIDTH > currentZoneDisplay[i].x && rect.x < currentZoneDisplay[i].x + currentZone[i].w && rect.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) {
	    return 1;
        }
    }
    return 0;
}

int colliding_platform(collision_type collidingWithType, SDL_Rect obj) {
    /* Initialize our returnCollisionType variable */
    collision_type returnCollisionType = COLLISION_NOT_FOUND;

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Ensure we are on the same y level as the block */
        if ((obj.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) && (obj.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y)) {
            /* Up / Down collisions */
            if (obj.x + PLAYER_WIDTH > currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x + currentZone[i].w) {
                if ((obj.y + (obj.h / 2)) < (currentZoneDisplay[i].y + (currentZoneDisplay[i].h / 2))) {
	            returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_UP);
                } else {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_DOWN);
                }
            }
            if ((obj.y != (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) && (obj.y + PLAYER_HEIGHT != currentZoneDisplay[i].y)) {
                /* Left / Right collisions */
                /* Check for Left Collision */
                if (obj.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x) {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_LEFT);
                }
                /* Check for Right Collision */
                if (obj.x + PLAYER_WIDTH > (currentZoneDisplay[i].x + currentZoneDisplay[i].w) && obj.x <= (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_RIGHT);
                }
            }
        }
        if (returnCollisionType & collidingWithType) {
            return i;
        }
    }
    return -1;
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
        /* Ensure we are on the same y level as the block */
        if ((rect.y <= (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) && (rect.y + PLAYER_HEIGHT >= currentZoneDisplay[i].y)) {
            /* Up / Down collisions */
            if (rect.x + PLAYER_WIDTH > currentZoneDisplay[i].x && rect.x < currentZoneDisplay[i].x + currentZone[i].w) {
                if ((rect.y + (rect.h / 2)) < (currentZoneDisplay[i].y + (currentZoneDisplay[i].h / 2))) {
	            returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_UP);
                } else {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_DOWN);
                }
            }
            /* Left / Right collisions */
            if ((rect.y != (currentZoneDisplay[i].y + currentZoneDisplay[i].h)) && (rect.y + PLAYER_HEIGHT != currentZoneDisplay[i].y)) {
                /* Check for Left Collision */
                if (rect.x + PLAYER_WIDTH >= currentZoneDisplay[i].x && rect.x < currentZoneDisplay[i].x) {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_LEFT);
                }
                /* Check for Right Collision */
                if (rect.x + PLAYER_WIDTH > (currentZoneDisplay[i].x + currentZoneDisplay[i].w) && rect.x <= (currentZoneDisplay[i].x + currentZoneDisplay[i].w)) {
                    returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_RIGHT);
                }
            }
        }
    }
    return returnCollisionType;
}

collision_type get_collision_type_for_platform(int platformIndex, SDL_Rect obj) {
    /* Make sure platformIndex exists before calling function */

    /* Initialize our returnCollisionType variable */
    collision_type returnCollisionType = COLLISION_NOT_FOUND;

    /* Ensure we are on the same y level as the block */
    if (obj.y <= (currentZoneDisplay[platformIndex].y + currentZoneDisplay[platformIndex].h) && obj.y + PLAYER_HEIGHT >= currentZoneDisplay[platformIndex].y) {
        /* Up / Down collisions */
        if (obj.x + PLAYER_WIDTH > currentZoneDisplay[platformIndex].x && obj.x < currentZoneDisplay[platformIndex].x + currentZone[platformIndex].w) {
            if (obj.y + (obj.h / 2) < currentZoneDisplay[platformIndex].y + (currentZoneDisplay[platformIndex].h / 2)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_UP);
            } else {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_DOWN);
            }
        }
        /* Left / Right collisions */
        if ((obj.y != (currentZoneDisplay[platformIndex].y + currentZoneDisplay[platformIndex].h)) && (obj.y + PLAYER_HEIGHT != currentZoneDisplay[platformIndex].y)) {
            /* Check for Left Collision */
            if (obj.x + PLAYER_WIDTH >= currentZoneDisplay[platformIndex].x && obj.x < currentZoneDisplay[platformIndex].x) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_LEFT);
            }
            /* Check for Right Collision */
            if (obj.x + PLAYER_WIDTH > (currentZoneDisplay[platformIndex].x + currentZoneDisplay[platformIndex].w) && obj.x <= (currentZoneDisplay[platformIndex].x + currentZoneDisplay[platformIndex].w)) {
                returnCollisionType = (returnCollisionType | COLLISION_IS_TOUCHING_RIGHT);
            }
        }
    }
    return returnCollisionType;
}

/* Returns below platform */
int below_platform(SDL_Rect obj) {
    /* Initialize our returnBelowPlatform variable */
    int returnBelowPlatform = -1;
    if (!currentZoneDisplay) {
        /* ERROR! No platforms?? */
        return -1;
        
    }
    int yOfLastBelowPlatform = INT_MAX;

    /* Get bottom of player */
    //int playerBottom = (obj.y + PLAYER_HEIGHT);

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Ensure we are on the same x level as the block */
        if (obj.x + PLAYER_WIDTH > currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x + currentZone[i].w) {
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
            if ((currentZoneDisplay[i].y + currentZoneDisplay[i].h) > (obj.y + 2)) {
#else
            if ((currentZoneDisplay[i].y + currentZoneDisplay[i].h) > obj.y) {
#endif
                /* Make sure this platform is above the last platform we checked */
                if (currentZoneDisplay[i].y < yOfLastBelowPlatform) {
                    yOfLastBelowPlatform = currentZoneDisplay[i].y;
                    returnBelowPlatform = i;
                }
            }
        }
    }
    return returnBelowPlatform;
}

/* Returns above platform, reverse of below_platform */
int above_platform(SDL_Rect obj) {
    /* Initialize our returnBelowPlatform variable */
    int returnAbovePlatform = -1;
    int yOfLastAbovePlatform = INT_MIN;

    /* Get bottom of player */
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
    int playerBottom = (obj.y + PLAYER_HEIGHT) - 2;
#else
    int playerBottom = (obj.y + PLAYER_HEIGHT);
#endif

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Ensure we are on the same x level as the block */
        if (obj.x + PLAYER_WIDTH > currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x + currentZone[i].w) {
            if (currentZoneDisplay[i].y < playerBottom) {
                /* Make sure this platform is above the last platform we checked */
                if (currentZoneDisplay[i].y > yOfLastAbovePlatform) {
                    yOfLastAbovePlatform = currentZoneDisplay[i].y;
                    returnAbovePlatform = i;
                }
            }
        }
    }
    return returnAbovePlatform;
}

/* 
 * If the player is partly in a platform somehow, below_platform detects
 * that platform as below the player. below_platform_nointersect is below_platform
 * but without this. Be aware that if a player is fully in a platform however,
 * below_platform_nointersect *will* detect the platform as below.
*/
int below_platform_nointersect(SDL_Rect obj) {
    /* Initialize our returnBelowPlatform variable */
    int returnBelowPlatform = -1;
    int yOfLastBelowPlatform = INT_MAX;

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Ensure we are on the same x level as the block */
        if (obj.x + PLAYER_WIDTH > currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x + currentZone[i].w) {
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
            if (currentZoneDisplay[i].y > (obj.y + 2)) {
#else
            if (currentZoneDisplay[i].y > obj.y) {
#endif
                /* Make sure this platform is above the last platform we checked */
                if (currentZoneDisplay[i].y < yOfLastBelowPlatform) {
                    yOfLastBelowPlatform = currentZoneDisplay[i].y;
                    returnBelowPlatform = i;
                }
            }
        }
    }
    return returnBelowPlatform;
}

/* 
 * If the player is partly in a platform somehow, above_platform detects
 * that platform as above the player. above_platform_nointersect is above_platform
 * but without this. Be aware that if a player is fully in a platform however,
 * above_platform_nointersect *will* detect the platform as above.
*/
int above_platform_nointersect(SDL_Rect obj) {
    /* Initialize our returnBelowPlatform variable */
    int returnAbovePlatform = -1;
    int yOfLastAbovePlatform = INT_MIN;

    /* Get bottom of player */
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
    int playerBottom = obj.y - 2;
#else
    int playerBottom = obj.y;
#endif

    /* Get collision type for platform */
    for (int i = 0; i < platformCount; i++) {
        /* Ensure we are on the same x level as the block */
        if (obj.x + PLAYER_WIDTH > currentZoneDisplay[i].x && obj.x < currentZoneDisplay[i].x + currentZone[i].w) {
            if (currentZoneDisplay[i].y < playerBottom) {
                /* Make sure this platform is above the last platform we checked */
                if (currentZoneDisplay[i].y > yOfLastAbovePlatform) {
                    yOfLastAbovePlatform = currentZoneDisplay[i].y;
                    returnAbovePlatform = i;
                }
            }
        }
    }
    return returnAbovePlatform;
}

/*
 * Returns below platform at index.
 * Unlike below_platform, this function does not check the x of the platform, only the y.
*/
int below_platform_at_index(int platformIndex, SDL_Rect obj) {
    SDL_Rect platform = currentZoneDisplay[platformIndex];
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
    if ((platform.y + platform.h) > (obj.y + 2)) {
#else
    if ((platform.y + platform.h) > obj.y) {
#endif
        return 1; /* We're below the platform, return 1 */
    }
    return 0;
}

/*
 * Returns above platform at index.
 * Unlike above_platform, this function does not check the x of the platform, only the y.
*/
int above_platform_at_index(int platformIndex, SDL_Rect obj) {
    SDL_Rect platform = currentZoneDisplay[platformIndex];
    /* Get bottom of player */
#if HACKY_WORKAROUND_FOR_COLLISION_BUG
    int playerBottom = (obj.y + PLAYER_HEIGHT) - 2;
#else
    int playerBottom = (obj.y + PLAYER_HEIGHT);
#endif
    if (platform.y < playerBottom) {
        return 1; /* We're above the platform, return 1 */
    }
    return 0;
}