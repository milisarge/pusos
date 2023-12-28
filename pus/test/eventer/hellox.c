#include <stdio.h>
#include <string.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <lib/helloi.h>

#include <lib/app600.h>

static void hellox(const struct app_descriptor *app, void *args) {
	
	printf("signaling event\n");
	if (mu_isnil(scope_600)){
		scope_600= mu_tbl_extend(0, MU_BUILTINS);
	}
	char *cmd = "let data=['value':3]";
	mu_eval(cmd, strlen(cmd), scope_600, 0);
    event_signal(&e_hello, true);
    while(e_hello.signaled){
	  printf("event is processing\n");
	}
    printf("done signaling event\n");
	
	/*
	thread_sleep(1);
	event_unsignal(&e_hello);
	printf("unsignaled event-hello eventer:%p\n",get_current_thread());
	*/
}

static void hellox_init(const struct app_descriptor *app){
    printf("hellox init\n");
    //event_init(&e_hello, false, 0);
}

APP_START(hellox)
.init = hellox_init,
.entry = hellox,
.flags = 1,
APP_END

