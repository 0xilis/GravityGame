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

#ifndef ZONES_H
#define ZONES_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "defs.h"

SDL_Rect *currentZoneDisplay;
SDL_Rect *currentZone;

void load_test_zone();
int platformsInCurrentZone();
SDL_Rect * load_level_as_full_zone(const char *restrict filename);

#endif //ZONES_H