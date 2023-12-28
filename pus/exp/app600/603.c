/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>
#include <lk/err.h>

#include <lib/minip.h>
#include <lk/trace.h>

// coap lib
#include <lib/coap.h>
// bio device ops
#include <lib/bio.h>
//generate msg ids
#include <rand.h>
#include <platform/rtc_time.h>
#include <lib/datetime.h>

#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"

#include "app600.h"

#define RULE_isHot "any( fn(x) -> "\
				   "if (temp[x] > 49) print(temp[x] ,\":hot\") "\
				   "else print(temp[x] ,\":not hot\")" \
				   ",pairs(temp))"

static int app603(int argc, const console_cmd_args *argv) {
	mutex_acquire(&mu_lock_600);
    printf("temp evaluate;\n");
    char *rule = RULE_isHot ;
    mu_eval(rule, strlen(rule), scope_600, 0);
	mutex_release(&mu_lock_600);
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app603", "app603 tests", &app603)
STATIC_COMMAND_END(app603);

APP_START(app603)
APP_END

