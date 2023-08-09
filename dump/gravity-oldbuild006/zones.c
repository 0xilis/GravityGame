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
#include "seajson.h"

/* Platform dimensions */
#define PLATFORM_WIDTH 200
#define PLATFORM_HEIGHT 20

int currentZonePlatformCount = 0;

#if 0
void load_test_zone(SDL_Rect *currentZone) {
    SDL_Rect platform1 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2, SCREEN_HEIGHT - PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[0] = platform1;
    int platform1_right_endpoint = (SCREEN_WIDTH/2 - PLATFORM_WIDTH/2) + PLATFORM_WIDTH;
    SDL_Rect platform2 = {platform1_right_endpoint + 50, SCREEN_HEIGHT - PLATFORM_HEIGHT - 50, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[1] = platform2;
    SDL_Rect platform3 = {platform1_right_endpoint + 200 + PLATFORM_WIDTH, SCREEN_HEIGHT - PLATFORM_HEIGHT - 50, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[2] = platform3;

    SDL_Rect platform4 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2, SCREEN_HEIGHT - PLATFORM_HEIGHT - 300, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[3] = platform4;
    SDL_Rect platform5 = {platform1_right_endpoint + 50, SCREEN_HEIGHT - PLATFORM_HEIGHT - 350, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[4] = platform5;
    SDL_Rect platform6 = {platform1_right_endpoint + 200 + PLATFORM_WIDTH, SCREEN_HEIGHT - PLATFORM_HEIGHT - 350, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[5] = platform6;

    /* The platform to test the collision */
    SDL_Rect platform7 = {platform1_right_endpoint + 250 + PLATFORM_WIDTH, SCREEN_HEIGHT - PLATFORM_HEIGHT - 70, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[6] = platform7;

    SDL_Rect platform8 = {platform1_right_endpoint + 250 + PLATFORM_WIDTH, SCREEN_HEIGHT - PLATFORM_HEIGHT - 20, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[7] = platform8;

    /* The platform to test the collision for _RIGHT */
    SDL_Rect platform9 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2 - PLATFORM_WIDTH - 80, SCREEN_HEIGHT - PLATFORM_HEIGHT - 70, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[8] = platform9;

    SDL_Rect platform10 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2 - PLATFORM_WIDTH - 80, SCREEN_HEIGHT - PLATFORM_HEIGHT - 20, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[9] = platform10;

    SDL_Rect platform11 = {SCREEN_WIDTH/2 - PLATFORM_WIDTH/2 - PLATFORM_WIDTH - 50, SCREEN_HEIGHT - PLATFORM_HEIGHT - 50, PLATFORM_WIDTH, PLATFORM_HEIGHT};
    currentZone[10] = platform11;

    currentZonePlatformCount = platforms_in_test_zone();
}
#endif

SDL_Rect * load_level_as_full_zone(const char *restrict filename) {
    printf("loading level \"%s\" ...\n",filename);
    seajson level = init_json_from_file(filename);
    currentZonePlatformCount = get_int(level,"platform_count");
    SDL_Rect *currentZone = (SDL_Rect *)malloc(sizeof(SDL_Rect) * currentZonePlatformCount);
    jarray platforms = get_array(level,"platforms");
    int platformCount = platforms.itemCount;
    for (int i = 0; i < platformCount; i++) {
        seajson platformInfo = get_item_from_jarray(platforms, i);
        jarray platformPointTemp = get_array(platformInfo,"point");
        jarray platformPoint = remove_whitespace_from_jarray(platformPointTemp);
        /* Free the jarray with whitespace since it is unusable */
        free_jarray(platformPointTemp);
        SDL_Rect platform = {get_int_from_jarray(platformPoint, 0), get_int_from_jarray(platformPoint, 1), get_int_from_jarray(platformPoint, 2), get_int_from_jarray(platformPoint, 3)};
        currentZone[i] = platform;
        free(platformInfo);
        free_jarray(platformPoint);
    }
    free_jarray(platforms);
    free(level);
    printf("loaded \"%s\".\n",filename);
    return currentZone;
}

int platformsInCurrentZone() {
    return currentZonePlatformCount;
}