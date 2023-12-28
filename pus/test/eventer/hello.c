#include <stdio.h>
#include <string.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>
//#include "helloi.h"
//#include <lib/helloi.h>
#include <kernel/event.h>
event_t e_hello;

#include <lib/app600.h>

static void hello(const struct app_descriptor *app, void *args) {

	//printf("hello:event is waiting...\n");
	/*
	while(1){
		if (event_wait(&e_hello) < 0) {
			printf("%p: event_wait() returned error\n", get_current_thread());
		}
		printf("this is hello app:%p\n",get_current_thread());
	}
	*/
	while(1){
		if (event_wait_timeout(&e_hello, 4000) < 0) {
			//printf("%p: event_wait() timeout\n", get_current_thread());
			thread_sleep(3000);
		} else {
			printf("this is signalled hello app:%p\n",get_current_thread());
			thread_sleep(5);
			char *cmd = "print(data.value)";
			mu_eval(cmd, strlen(cmd), scope_600, 0);
			event_unsignal(&e_hello);
		}
	}
}

static void hello_init(const struct app_descriptor *app){
    printf("hello init\n");
    event_init(&e_hello, false, 0);
}

APP_START(hello)
.init = hello_init,
.entry = hello,
.flags = 0,
APP_END

