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

Mix_Music *music;

void play_music(void) {
    /* Initialize SDL_mixer */
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "could not initialize SDL_mixer: %s\n", Mix_GetError());
        return;
    }

    /* Load background music */
    music = Mix_LoadMUS("resources/sonic_3_at_3am.wav");
    if (!music) {
        fprintf(stderr, "could not load background music: %s\n", Mix_GetError());
        return;
    }

    /* Play music continuously */
    if (Mix_PlayMusic(music, -1) < 0) {
        fprintf(stderr, "could not play background music: %s\n", Mix_GetError());
    }
}

Mix_Chunk* chunk;

void jump_sound(void) {
    if (!chunk) {
        chunk = Mix_LoadWAV("resources/jump.mp3");
    }
    Mix_PlayChannel(-1, chunk, 0);
}

/* ran before exit, cleans up audio */
void cleanup_audio(void) {
    Mix_FreeChunk(chunk);
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}