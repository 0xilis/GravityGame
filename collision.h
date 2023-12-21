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

#ifndef COLLISION_H
#define COLLISION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include "sound.h"
#include "zones.h"
#include "resource_management.h"
#include <limits.h>
#include "collision.h"

#include "defs.h"

/* Global variables */
#define STARTING_PLATFORM_HEIGHT 200
extern SDL_Rect rect;
int platformCount;

/* typedef */
typedef int collision_type;

/* Collision functions */
int check_platform_collision(void);
int colliding_platform(collision_type collidingWithType, SDL_Rect obj);
collision_type get_collision_type(void);
collision_type get_collision_type_for_platform(int platformIndex, SDL_Rect obj);
int below_platform(SDL_Rect obj);
int above_platform(SDL_Rect obj);
int below_platform_nointersect(SDL_Rect obj);
int above_platform_nointersect(SDL_Rect obj);
int below_platform_at_index(int platformIndex, SDL_Rect obj);
int above_platform_at_index(int platformIndex, SDL_Rect obj);

/* Collision */
#define HACKY_WORKAROUND_FOR_COLLISION_BUG 1
#define COYOTE_MAX 5

#endif //COLLISION_H
