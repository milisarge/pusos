/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <app.h>
#include <lk/err.h>
#include <lk/debug.h>
#include <lk/trace.h>
#include <stdio.h>
#include <stdlib.h>
#include <lk/compiler.h>
#include <kernel/thread.h>
#include <lib/minip.h>
#include <lib/cksum.h>
#include <platform.h>

#include "../coapsrv/coapsrv.h"

static int wc_worker(void *socket) {
  
  
   tcp_socket_t *s = socket;

#define BUFSIZE 512

    //uint8_t *buf = malloc(WORDCOUNT_BUFSIZE);
    char *buf = malloc(BUFSIZE);
    printf("coming data len:%ld\n",sizeof(buf));
    if (!buf)
        return ERR_NO_MEMORY;
    const char *resp="data is ok\n";
    //int counter=0;
    for (;;) {
        ssize_t ret = tcp_read(s, buf, 256);
        //printf("%d-%s-%d\n",counter++,buf,sizeof(buf));
        //printf("----\n");
        /*
        for (int i=0;i<256;i++){
			printf("%c",*(buf+i));
			if (*(buf+i) == '!')  break;
		}
		*/
		printf("data: %s\n",buf);
        if (ret <= 0)
            break;

        tcp_write(s, resp, sizeof(resp));
        if (ret <= 0)
            break;
    }
    //tcp_write(s, buf, WORDCOUNT_BUFSIZE);
    free(buf);
    printf("buffer temizlendi\n");
    tcp_close(s);

    return 0;
  
  
}

static int wc_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;

    err = tcp_open_listen(&listen_socket, 4443);
    printf("wc basladi\n");
    if (err < 0) {
        TRACEF("error opening wc listen socket\n");
        return -1;
    }

    for (;;) {
        tcp_socket_t *accept_socket;

        err = tcp_accept(listen_socket, &accept_socket);
        TRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
        if (err < 0) {
            TRACEF("error accepting socket, retrying\n");
            continue;
        }

        TRACEF("starting wc worker\n");
        thread_detach_and_resume(thread_create("wc_worker", &wc_worker, accept_socket, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    }
}

static int echo_worker(void *socket) {
    tcp_socket_t *s = socket;

#define ECHO_BUFSIZE 1024

    uint8_t *buf = malloc(ECHO_BUFSIZE);
    if (!buf) {
        TRACEF("error allocating buffer\n");
        return ERR_NO_MEMORY;
    }

    for (;;) {
        ssize_t ret = tcp_read(s, buf, sizeof(buf));
        if (ret <= 0)
            break;

        tcp_write(s, buf, ret);
        if (ret <= 0)
            break;
    }

    TRACEF("echo worker exiting\n");
    tcp_close(s);
    free(buf);

    return 0;
}

static int echo_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;

    err = tcp_open_listen(&listen_socket, 4442);
    if (err < 0) {
        TRACEF("error opening echo listen socket\n");
        return -1;
    }

    for (;;) {
        tcp_socket_t *accept_socket;

        err = tcp_accept(listen_socket, &accept_socket);
        TRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
        if (err < 0) {
            TRACEF("error accepting socket, retrying\n");
            continue;
        }

        TRACEF("starting echo worker\n");
        thread_detach_and_resume(thread_create("echo_worker", &echo_worker, accept_socket, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    }
}

static void netsrv_init(const struct app_descriptor *app) {
	// servislerin başlama yeri gibi düşünerek
	
	// statik ip tanımlamak
	/*
	printf("setting static IP address\n");
	uint32_t ip_addr = IPV4(192, 168, 122, 99);
	uint32_t ip_mask = IPV4(255, 255, 255, 0);
	uint32_t ip_gateway = IPV4(192, 168, 122, 1);
	minip_start_static(ip_addr, ip_mask, ip_gateway);
	*/
}

static void netsrv_entry(const struct app_descriptor *app, void *args) {
    /* XXX wait for the stack to initialize */

    printf("starting my internet servers\n");

    thread_detach_and_resume(thread_create("Network_Service", &wc_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    //thread_detach_and_resume(thread_create("discard", &discard_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    //thread_detach_and_resume(thread_create("echo", &echo_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    coapsrv_init(NULL);
}

APP_START(netsrv)
.init = netsrv_init,
.entry = netsrv_entry,
.flags = 0,
APP_END
