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

SDL_Rect * load_level_as_full_zone(const char *restrict filename) {
    printf("loading level \"%s\" ...\n",filename);
    seajson level = init_json_from_file(filename);
    jarray platforms = get_array(level,"platforms");
    free(level);
    currentZonePlatformCount = platforms.itemCount;
    SDL_Rect *currentZone = (SDL_Rect *)malloc(sizeof(SDL_Rect) * currentZonePlatformCount);
    for (int i = 0; i < currentZonePlatformCount; i++) {
        seajson platformInfo = get_item_from_jarray(platforms, i);
        jarray platformPointTemp = get_array(platformInfo,"point");
        free(platformInfo);
        jarray platformPoint = remove_whitespace_from_jarray(platformPointTemp);
        /* Free the jarray with whitespace since it is unusable */
        free_jarray(platformPointTemp);
        SDL_Rect platform = {get_int_from_jarray(platformPoint, 0), get_int_from_jarray(platformPoint, 1), get_int_from_jarray(platformPoint, 2), get_int_from_jarray(platformPoint, 3)};
        free_jarray(platformPoint);
        currentZone[i] = platform;
    }
    free_jarray(platforms);
    printf("loaded \"%s\".\n",filename);
    return currentZone;
}

SDL_Rect * load_level_as_full_zone_display(const char *restrict filename, SDL_Rect **display) {
    printf("loading level \"%s\" ...\n",filename);
    seajson level = init_json_from_file(filename);
    jarray platforms = get_array(level,"platforms");
    free(level);
    currentZonePlatformCount = platforms.itemCount;
    SDL_Rect *currentZone = (SDL_Rect *)malloc(sizeof(SDL_Rect) * currentZonePlatformCount);
    SDL_Rect *currentZoneDisplay = (SDL_Rect *)malloc(sizeof(SDL_Rect) * currentZonePlatformCount);
    for (int i = 0; i < currentZonePlatformCount; i++) {
        seajson platformInfo = get_item_from_jarray(platforms, i);
        jarray platformPointTemp = get_array(platformInfo,"point");
        free(platformInfo);
        jarray platformPoint = remove_whitespace_from_jarray(platformPointTemp);
        /* Free the jarray with whitespace since it is unusable */
        free_jarray(platformPointTemp);
        SDL_Rect platform = {get_int_from_jarray(platformPoint, 0), get_int_from_jarray(platformPoint, 1), get_int_from_jarray(platformPoint, 2), get_int_from_jarray(platformPoint, 3)};
        free_jarray(platformPoint);
        currentZone[i] = platform;
        currentZoneDisplay[i] = platform;
    }
    free_jarray(platforms);
    printf("loaded \"%s\".\n",filename);
    *display = currentZoneDisplay;
    return currentZone;
}

int platformsInCurrentZone() {
    return currentZonePlatformCount;
}