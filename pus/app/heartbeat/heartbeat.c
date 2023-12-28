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


#define LOCAL_TRACE 0

typedef struct {
  int id;
  lk_time_t wait_time;	
} worker_arg;

static int heartbeat_worker(void *targ) {
    worker_arg warg = *(worker_arg *) targ;
    uint32_t server;
    uint16_t port;
    status_t err;
    server = IPV4(192,168,122,1);
    port = 5555;
    tcp_socket_t *sock ;
    uint8_t readbuf[32];
    unsigned short i;
  
    for (;;) { 
        err = tcp_connect(&sock, server, port);
        if (err < 0) {
          LTRACEF("error connecting to socket\n");
          //return -1;
        }
		err = tcp_write(sock, "ACTIVE\n", strlen("ACTIVE\n"));
		if (err < 0) {
		  LTRACEF("error writing to socket\n");
		  
		  //return -1;
		}
		err = tcp_read(sock, readbuf, 32);
		if (err < 0) {
		  LTRACEF("error reading from socket\n");
		  
		  //return -1;
		} 
		// gelen cevabı okumak
		/*
		else {
		  i = 0;
   		  while( i < sizeof(readbuf )) {
			printf("%c",readbuf[i]);
			if(readbuf[++i] == '!')
			  break;
		  }
	      printf("\n");
	    }
	    */
        // bekleme süresi
        //printf("hb wait start\n");
        // close seocket
        tcp_close(sock);
	    thread_sleep(warg.wait_time);
	    //printf("hb wait ended\n");
    }

    return 0;
}

//static void heartbeat(int argc, const console_cmd_args *argv) {
static void heartbeat(const struct app_descriptor *app, void *args) {
    while(true) {
	    // ping alma ile düzelt
	    // statikte bu işe yaramadı
	    if ( (minip_get_ipaddr() & 0xFF) != 0 ){
			break;
		}
		thread_sleep(1000);
		LTRACEF("1 sn bekleyip ip kontrolu yapilacak.\n");
	}	
    //printf("starting heartbeat thread\n");
    worker_arg warg;
    warg.id = 0;
    //console_cmd_args * deneme = (console_cmd_args *)args;
    //printf("denem_t:%d\n",deneme->i);
    // merkez bulut hazır değilse uyut
    // thread_sleep(2000)
    warg.wait_time = 5000;
    
    thread_detach_and_resume(thread_create("Heartbeat_Service", &heartbeat_worker, (void *) &warg, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

static void heartbeat_init(const struct app_descriptor *app){
    printf("heartbeat service init\n");
}

//STATIC_COMMAND_START
//STATIC_COMMAND("heartbeat", "heartbeat tests", &heartbeat)
//STATIC_COMMAND_END(heartbeat);

APP_START(heartbeat)
.init = heartbeat_init,
.entry = heartbeat,
.flags = 0,
APP_END


