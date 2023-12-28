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

#include <lib/minip.h>
#include <lk/trace.h>
#include "httpserver.h"

#define LOCAL_TRACE 1

#define HTTP_PORT 7777

static int http_worker(void *socket) {
    int i;
    int err;
    tcp_socket_t *sock = socket;

#define BUFSIZE 1024

    uint8_t *readbuf = malloc(BUFSIZE);
    if (!readbuf) {
        LTRACEF("error allocating buffer\n");
        return ERR_NO_MEMORY;
    }
	for (;;) {
		//thread_sleep(500);
		err = tcp_read(sock, readbuf, BUFSIZE);
		if (err < 0) {
		  LTRACEF("error reading from socket:%d\n",err);
		  break;
		} 
		// gelen cevabı okumak
		else {
		  i = 0;
		  while( i < sizeof(readbuf )) {
			//printf("%c",readbuf[i]);
			if(readbuf[++i] == '!')
			  break;
		  }
		  // parse req
		  parse_request(readbuf, sock);
		  
		  //printf("..\n");
		  /*
		  err = tcp_write(sock, "GOT_IT\n", strlen("GOT_IT\n"));
		  if (err < 0) {
			  LTRACEF("error writing to socket\n");
			  break;
		  }
		  * */
		}
	}

    LTRACEF("http worker exiting\n");
    tcp_close(sock);
    free(readbuf);

    return 0;
}

static void http_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;

    err = tcp_open_listen(&listen_socket, HTTP_PORT);
    if (err < 0) {
        LTRACEF("error opening echo listen socket\n");
        return;
    }

    for (;;) {
        tcp_socket_t *accept_socket;

        err = tcp_accept(listen_socket, &accept_socket);
        LTRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
        if (err < 0) {
            TRACEF("error accepting socket, retrying\n");
            continue;
        }

        LTRACEF("starting http worker\n");
        thread_detach_and_resume(thread_create("Pus_API_Worker", &http_worker, accept_socket, DEFAULT_PRIORITY, 8196));
    }
}

static void http_entry(const struct app_descriptor *app, void *args) {
    thread_detach_and_resume(thread_create("Pus_API_Server", &http_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

APP_START(http)
.init = NULL,//http_init,
.entry = http_entry,
.flags = 1,
APP_END

