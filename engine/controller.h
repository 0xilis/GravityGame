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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>

/* TODO: Find actual deadzone */
#define CONTROLLER_DEADZONE 800

SDL_GameController *find_controller(void);

#endif /* RESOURCE_MANAGEMENT_H */