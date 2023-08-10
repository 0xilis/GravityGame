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

// thnk u https://stackoverflow.com/questions/2349776/how-can-i-benchmark-c-code-easily

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TEST_OPTIMIZATION 1
#define TEST_2_OPTIMIZATION 0

/*
y += (25.0 - jumpHeight);
 jumpHeight += (6.0 - (jumpHeight * 0.075));
*/
/*
y += (25.0 - jumpHeight);
 jumpHeight += (6.0 - (jumpHeight * 0.075));
*/

int main(void) {

clock_t startTime = clock();

/* Do work */

int y = 0;
int jumpHeight = 0;

long timesRun = 100000;

// clang speedtest.c -o speedtest
// ./speedtest

printf("starting speed test...\n");

#if TEST_2_OPTIMIZATION
for (long i = 0; i < timesRun; i++) {
 y += (25.0 - jumpHeight);
 jumpHeight += (6.0 - (jumpHeight * 0.075));
 if (jumpHeight >= 200) {
  jumpHeight = 0;
  y = 0;
 }
}
#else
#if TEST_OPTIMIZATION
for (long i = 0; i < timesRun; i++) {
 y += (25.0 - (0.3 * jumpHeight));
 jumpHeight += (20.0 - jumpHeight/4);
 if (jumpHeight >= 200) {
  jumpHeight = 0;
  y = 0;
 }
}
#else
for (long i = 0; i < timesRun; i++) {
 y += (20.0 - jumpHeight/4) + (5 - jumpHeight/20);
 jumpHeight += (20.0 - jumpHeight/4);
 if (jumpHeight >= 200) {
  jumpHeight = 0;
  y = 0;
 }
}
#endif
#endif

double elapsed_time = (double)(clock() - startTime) / CLOCKS_PER_SEC;

printf("elapsed_time: %f\n", elapsed_time);
}