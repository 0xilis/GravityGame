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
    seajson level_raw = init_json_from_file(filename);
    /* remove whitespace from json */
    seajson level = remove_whitespace_from_json(level_raw);
    free(level_raw);
    /* Things *should* be fine now... */
    
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

#if 0
/* Perhaps move this to a separate file, ex level_edit.c/level_debug.c/debug_tools.c */
void add_platform(SDL_Rect platform, const char *restrict levelname) {
 /* Probably a *bad* idea to reload the json every block placement, but right now I'm lazy... */
 jarray blankJarray = new_jarray();
 jarray pointWithX = add_item_to_jarray(blankJarray,platform.x);
 free_jarray(blankJarray);
 jarray pointWithXY = add_item_to_jarray(pointWithX,platform.y);
 free_jarray(pointWithX);
 jarray pointWithXYW = add_item_to_jarray(pointWithXY,platform.w);
 free_jarray(pointWithXY);
 jarray pointWithXYWH = add_item_to_jarray(pointWithXYW,platform.w);
 free_jarray(pointWithXYW);
 seajson tempPlatform = "{\"object_type\":\"generic_platform\",\"object_id\":0}";
 newPlatform = add_item_seajson(newPlatform,"point",pointWithXYWH.arrayString);
 free(tempPlatform);
 free_jarray(pointWithXYWH);

 /* newPlatform should now be the full platform to place */
 seajson level_raw = init_json_from_file(filename);
 /* remove whitespace from json */
 seajson level = remove_whitespace_from_json(level_raw);
 free(level_raw);
 /* Things *should* be fine now... */
 jarray platforms = get_array(level,"platforms");
 jarray newPlatforms = add_item_to_jarray(platforms, newPlatform);
 free_jarray(platforms);
 seajson newLevel = set_item_seajson(level, newPlatforms.arrayString);
 free(level);
 free_jarray(newPlatforms);
 /* newLevel should be the new json */

 /* write contexts of newLevel json over to new file */
 /* TODO: Error Handling... */
 FILE *fptr = fopen(levelname, "w");
 fprintf(fptr, newLevel);
 fclose(fptr);
}
#endif