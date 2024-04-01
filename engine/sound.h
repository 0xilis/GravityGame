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

#ifndef SOUND_H
#define SOUND_H

void play_music(char *resourcePath);
void jump_sound(void);
void cleanup_audio(void);
void pause_audio(void);
void resume_audio(void);

#endif /* SOUND_H */