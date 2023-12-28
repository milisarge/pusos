/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <rand.h>
#include <lk/err.h>
#include <app/tests.h>
#include <kernel/thread.h>
#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <kernel/event.h>
#include <platform.h>
#include <app.h>
#include "../../exp/raft1/app_raft.h"

int fib_seri[20];

static int fibon_thread(void *argv) {
    long fibon = (intptr_t)argv;

    thread_t *t[2];

    if (fibon == 0)
        return 0;
    if (fibon == 1)
        return 1;

    char name[32];
    snprintf(name, sizeof(name), "fibon %lu", fibon - 1);
    t[0] = thread_create(name, &fibon_thread, (void *)(fibon - 1), DEFAULT_PRIORITY, 2048);
    if (!t[0]) {
        printf("error creating thread for fibon %ld\n", fibon-1);
        return 0;
    }
    snprintf(name, sizeof(name), "fibon %lu", fibon - 2);
    t[1] = thread_create(name, &fibon_thread, (void *)(fibon - 2), DEFAULT_PRIORITY, 2048);
    if (!t[1]) {
        printf("error creating thread for fibon %ld\n", fibon-2);
        thread_resume(t[0]);
        thread_join(t[0], NULL, INFINITE_TIME);
        return 0;
    }

    thread_resume(t[0]);
    thread_resume(t[1]);

    int retcode0, retcode1;

    thread_join(t[0], &retcode0, INFINITE_TIME);
    thread_join(t[1], &retcode1, INFINITE_TIME);
	thread_sleep(1000);
	for(int i = 0; i < 20; i++){
        if (fib_seri[i] == retcode0 + retcode1)
          break;
        
        if((fib_seri[i] == 0) && (fib_seri[i] != retcode0 + retcode1) ){
          fib_seri[i] = retcode0 + retcode1;
          printf("%d ",fib_seri[i]);
          raft_sender("savevm",retcode0 + retcode1);
          break;
        }
    }
    
    return retcode0 + retcode1;
}

int fibon(int argc, const console_cmd_args *argv) {
	printf("%d ",1);
    for(int i = 0; i < 20; i++){
	  fib_seri[i] = 0;
	}
    if (argc < 2) {
        printf("not enough args\n");
        return -1;
    }

    lk_time_t tim = current_time();

    thread_t *t = thread_create("fibon", &fibon_thread, (void *)(uintptr_t)argv[1].u, DEFAULT_PRIORITY, 2048);
    thread_resume(t);

    int retcode;
    thread_join(t, &retcode, INFINITE_TIME);

    tim = current_time() - tim;

    //printf("\nfibon %d\n", retcode);
    printf("\ntook %u msecs to calculate\n", tim);

    return NO_ERROR;
}

STATIC_COMMAND_START
STATIC_COMMAND("fibon", "fibon tests", &fibon)
STATIC_COMMAND_END(fibon);

APP_START(fibon)
APP_END
