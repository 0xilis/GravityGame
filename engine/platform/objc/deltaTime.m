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

#import <Foundation/Foundation.h>
#include "deltaTime.h"

NSTimeInterval private_timeSince1970;

double get_delta_time(void) {
 NSTimeInterval currentTimeSince1970 = [[NSDate date] timeIntervalSince1970];
 double deltaTime = currentTimeSince1970 - private_timeSince1970;
 private_timeSince1970 = currentTimeSince1970;

 return deltaTime;
}
void init_delta_time(void) {
 private_timeSince1970 = [[NSDate date] timeIntervalSince1970];
}
