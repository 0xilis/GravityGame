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
#define SCREEN_WIDTH 854
#define SCREEN_HEIGHT 480

/* Player dimensions */
#define PLAYER_WIDTH 20 /* In the future, change to 40x40 when Physics better built for it */
#define PLAYER_HEIGHT 20
#define PLAYER_JUMP_VELOCITY 20.0
#define PLAYER_JUMP_MAX_HEIGHT 150.0
#define PLAYER_JUMP_VELOCITY_PLUS_GRAVITY 25.0

/* PlayerPos type */
typedef struct {
  int x;
  int y;
} PlayerPos;

#define GRAVITY 5
#define GRAVITY_MAX 25

/* Collision types */

#define COLLISION_IS_TOUCHING_UP 1
#define COLLISION_IS_TOUCHING_DOWN 2
#define COLLISION_IS_TOUCHING_LEFT 4
#define COLLISION_IS_TOUCHING_RIGHT 8
#define COLLISION_IS_TOUCHING_ANY 15 /* 1 | 2 | 4 | 8 = 15 */
#define COLLISION_NOT_FOUND 0

/* 
 * limits.h
 * For some reason, on my Arch Linux machine, it can't find limits.h.
 * Well, I should rephrase that: it *can*, but limits.h seems to
 * reference <linux/limits.h> which it cannot find. I know this
 * is a issue with my build set up rather than the code, and it
 * does work fine on my mac, but just in case I decided to instead
 * of including limits.h, define limits here manually myself :P.
*/
#define CHAR_MIN -127
#define CHAR_MAX 127
#define UCHAR_MAX 255
#define SHRT_MIN -32767
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN -2147483648
#define INT_MAX 2147483647
#define UINT_MAX 4294967295
#define LONG_MIN -2147483647
#define LONG_MAX 2147483647
#define ULONG_MAX 4294967295
#define LLONG_MIN -9223372036854775807
#define LLONG_MAX 9223372036854775807
#define ULLONG_MAX 18446744073709551615

#define FORCE_CONTROLLER 0

#endif /* DEFS_H */