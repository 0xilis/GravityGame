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

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "resource_management.h"

Mix_Music *music;

void play_music(char* resourcePath) {
    /* Initialize SDL_mixer */
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "could not initialize SDL_mixer: %s\n", Mix_GetError());
        return;
    }

    /* Load background music */
    char *musicResource = find_resource("resources/sonic_3_at_3am.wav");
    music = Mix_LoadMUS(musicResource);
    if (!music) {
        fprintf(stderr, "could not load background music: %s\n", Mix_GetError());
        return;
    }
    free(musicResource);

    /* Play music continuously */
    if (Mix_PlayMusic(music, -1) < 0) {
        fprintf(stderr, "could not play background music: %s\n", Mix_GetError());
    }
}

Mix_Chunk* chunk;
Mix_Chunk* blank;

void jump_sound(char* resourcePath) {
    if (!chunk) {
        char *jumpSfxResource = find_resource("resources/jump.mp3");
        chunk = Mix_LoadWAV(jumpSfxResource);
        free(jumpSfxResource);
        char *blankSfxResource = find_resource("resources/blank.mp3");
        blank = Mix_LoadWAV(blankSfxResource);
        free(blankSfxResource);
    }
    if (!Mix_Playing(3)) {
        /* SDL2's Mixer does not have native support for finding duration of chunk so here's my shitty workaround */
        Mix_PlayChannelTimed(3, blank, -1, 200);
        Mix_PlayChannel(-1, chunk, 0);
    }
}

/* ran before exit, cleans up audio */
void cleanup_audio(void) {
    Mix_FreeChunk(chunk);
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}

void pause_audio(void) {
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void resume_audio(void) {
    Mix_Resume(-1);
    Mix_ResumeMusic();
}