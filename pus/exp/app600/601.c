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

static int app601(int argc, const console_cmd_args *argv) {
	//scope_600= mu_tbl_extend(0, MU_BUILTINS);
	//mutex_init(&mu_lock_600);
	mutex_acquire(&mu_lock_600);
	// iterativ yazdırma any ile
    char *cmd = "any(fn(x,y)->print(x,':',y),pairs(temp));if (humidity) any(fn(x,y)->print(x,':',y),pairs(humidity))";
    //char *cmd = "print(temp);print(humidity)";
    mu_eval(cmd, strlen(cmd), scope_600, 0);
	mutex_release(&mu_lock_600);
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app601", "app601 tests", &app601)
STATIC_COMMAND_END(app601);

APP_START(app601)
APP_END

