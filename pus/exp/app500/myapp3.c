#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>

#include <lib/minip.h>
#include <lib/bio.h>
#include <lib/littlefs.h>
#include <rand.h>

#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"

mutex_t mu_lock_503;

mu_t scope_503;

int mu_worker_1503 () {
	while(true){
		mutex_acquire(&mu_lock_503);
		//scope = ((mu_t *)scope_p);
		char *cmd = "sayi=222;rakam=7;";
		mu_eval(cmd, strlen(cmd), scope_503, 0);
		mutex_release(&mu_lock_503);
		
		// key value şeklinde yazdırma
		/*
		mu_t b = mu_buf_create(0);
		muint_t n = 0;
		mu_t v;
		mu_t k;
		for (muint_t i = 0; mu_tbl_next(scope_503, &i, &k, &v);) {
			mu_buf_format(&b, &n, "%m:", k);
			mu_buf_format(&b, &n, "%m", v);
			
		}
		printf("%d-%s-\n",n, mu_buf_getdata(b));
		mu_buf_dec(b);
		*/
		thread_sleep(6000);
	}
	return 0;
}

int mu_worker_2503 () {
	while(true){
		mutex_acquire(&mu_lock_503);
		char *cmd = "print(sayi)";
		mu_eval(cmd, strlen(cmd), scope_503, 0);
		mutex_release(&mu_lock_503);
		thread_sleep(1000);
	}
	return 0;
}

static int app503(int argc, const console_cmd_args *argv) {
    scope_503= mu_tbl_extend(0, MU_BUILTINS);
    mutex_init(&mu_lock_503);
    thread_detach_and_resume(thread_create("muapp_worker1", &mu_worker_2503, scope_503, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    thread_detach_and_resume(thread_create("muapp_worker2", &mu_worker_1503, scope_503, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    
    
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app503", "app503 tests", &app503)
STATIC_COMMAND_END(app503);

APP_START(app503)
APP_END

