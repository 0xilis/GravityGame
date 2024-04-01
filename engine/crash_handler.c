/*
 * Copyright (C) 2024 Snoolie K / 0xilis. All rights reserved.
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
#include <string.h>
#include <signal.h>
#include "modding.h"

#define GRAVITY_RECOVER_ACTION_UNLOAD_MODS 0x1
#define GRAVITY_RECOVER_ACTION_UNLOAD_SOME_MODS 0x2

static __attribute__((always_inline)) void handle_bad_mod_fault_crash() {
 /*
  * we got an illegal instruction
  * very likely a modding issue
  * ex, a mod attempting to use another arch
  * try to restart with mods disabled
  * if mods are already disabled - well fuck
 */
 if (mods_enabled()) {
  force_disable_mods();
  disallow_mods(); /* so mods won't reload upon restart */
  game_recover(GRAVITY_RECOVER_ACTION_UNLOAD_MODS); /* attempt game recover... */
 } else {
  /* mods already disabled - uh oh */
  game_panic_screen();
 }
}

static __attribute__((always_inline)) void handle_possibly_our_fault_crashes() {
 /*
  * disable a mod, restart
  * and *hope* that that mod was causing issues
  * just do the last loaded mod in the mod list
 */
 if (mods_enabled()) {
  int loadedMods = mods_load_count();
  if (loadedMods) {
   permanent_disable_mod_at_index(loadedMods - 1);
   game_recover(GRAVITY_RECOVER_ACTION_UNLOAD_SOME_MODS);
  } else {
   /* no loader mods but modding enabled ??? */
   /* disable mods */
   force_disable_mods(); /* JUSTTTT in case... */
   disallow_mods();
   game_recover(GRAVITY_RECOVER_ACTION_UNLOAD_SOME_MODS);
  }
 } else {
  /* mods not enabled */
  /* is the game launched in safe mode? if not, try that IG... */
  if (is_safe_mode()) {
   /* we're in safe mode; give up, just panic at this point... */
   game_panic_screen();
  }
  game_recover_and_enable_safe_mode();
 }
}

void crash_handler(int sig) {
 if (sig == SIGILL) {
  handle_bad_mod_fault_crash();
 }
 if (sig == SIGSEGV || sig == SIGABRT) {
  handle_possibly_our_fault_crashes();
 }
 if (sig == SIGFPE) {
  if (mods_enabled()) {
   force_disable_mods();
   disallow_mods(); /* so mods won't reload upon restart */
   game_recover_and_enable_safe_mode(); /* attempt game recover... */
  } else {
   /* mods already disabled - uh oh */
   if (is_safe_mode()) {
    game_panic_screen();
   }
   game_recover_and_enable_safe_mode();
  }
 }
}