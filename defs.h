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

#ifndef DEFS_H
#define DEFS_H

/* Window dimensions */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

/* Player dimensions */
#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20
#define PLAYER_JUMP_VELOCITY 20.0
#define PLAYER_JUMP_MAX_HEIGHT 100.0
#define PLAYER_JUMP_HEIGHT_STAGE_2 80.0
#define PLAYER_JUMP_HEIGHT_STAGE_2_VELOCITY 10.0

/* PlayerPos type */
typedef struct {
  int x;
  int y;
} PlayerPos;

#define GRAVITY 5
#define GRAVITY_MAX 20

/* Collision types */

#define COLLISION_IS_TOUCHING_UP 1
#define COLLISION_IS_TOUCHING_DOWN 2
#define COLLISION_IS_TOUCHING_LEFT 4
#define COLLISION_IS_TOUCHING_RIGHT 8
#define COLLISION_NOT_FOUND 0
#define collision_type int

#endif //DEFS_H