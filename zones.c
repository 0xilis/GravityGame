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

#include "zones.h"

/* Platform dimensions */
#define PLATFORM_WIDTH 200
#define PLATFORM_HEIGHT 20

int currentZonePlatformCount = 0;

void load_test_zone(SDL_Rect *currentZone) {
    SDL_Rect platform1 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2, SCREEN_HEIGHT - PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[0] = platform1;
    SDL_Rect platform2 = {SCREEN_WIDTH/2 + PLATFORM_WIDTH/2 + 50, SCREEN_HEIGHT - PLATFORM_HEIGHT - 50, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[1] = platform2;
    currentZonePlatformCount = 2;
}

int platformsInCurrentZone() {
    return currentZonePlatformCount;
}