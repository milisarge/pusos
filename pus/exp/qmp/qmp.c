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


#define LOCAL_TRACE 1
//#define CAP_CMD "{"execute":"qmp_capabilities"}'

static int qmp(int argc, const console_cmd_args *argv) {
//static void qmp(const struct app_descriptor *app, void *args) {
    
    uint32_t server;
    uint16_t port;
    status_t err;
    server = IPV4(192,168,122,1);
    port = 4444;
    tcp_socket_t *sock ;
    uint8_t readbuf[1024] = {0};
    unsigned short i;


	err = tcp_connect(&sock, server, port);
	if (err < 0) {
	  LTRACEF("error connecting to socket\n");
	  return -1;
	}
	//printf("arg:%s-%d\n",argv[1].str,strlen(argv[1].str));
	//err = tcp_write(sock, "", 0);
	//thread_sleep(500);
	char * handshake = "{\"execute\":\"qmp_capabilities\"}";
	err = tcp_write(sock, handshake, 30);
	if (err < 0) {
	  LTRACEF("error writing to socket\n");
	  return -1;
	}
	thread_sleep(1000);
	err = tcp_read(sock, readbuf, 1024);
	if (err < 0) {
	  LTRACEF("error reading from socket\n");
	  return -1;
	} 
	else {
	  i = 0;
	  while( i < sizeof(readbuf )) {
		printf("%c",readbuf[i]);
		if(readbuf[++i] == 0)
		  break;
	  }
	  printf("\n"); 
	}
	thread_sleep(500);
	/////
	char *cmd;
	int cmd_len=82;
	
	if (strcmp(argv[1].str , "save") == 0){
		LTRACEF("saving.....\n");
		cmd = "{\"execute\":\"human-monitor-command\",\"arguments\":{\"command-line\":\"savevm state101\"}}";
	}
	if (strcmp(argv[1].str , "load") == 0){
		LTRACEF("loading....\n");
		cmd = "{\"execute\":\"human-monitor-command\",\"arguments\":{\"command-line\":\"loadvm state101\"}}";
	}
	memset(readbuf,0,1024);
	//err = tcp_write(sock, argv[1].str, strlen(argv[1].str)+1);
	err = tcp_write(sock, cmd, cmd_len);
	if (err < 0) {
	  LTRACEF("error writing to socket\n");
	  return -1;
	}
	thread_sleep(1000);
	err = tcp_read(sock, readbuf, 1024);
	if (err < 0) {
	  LTRACEF("error reading from socket\n");
	  return -1;
	} 
	else {
	  i = 0;
	  while( i < sizeof(readbuf )) {
		printf("%c",readbuf[i]);
		if(readbuf[++i] == 0)
		  break;
	  }
	  printf("\n");
	 
	  
	  
	}
	
	tcp_close(sock);
	return 0;
}

static void qmp_init(const struct app_descriptor *app){
    printf("qmp service init\n");
}

STATIC_COMMAND_START
STATIC_COMMAND("qmp", "qmp tests", &qmp)
STATIC_COMMAND_END(qmp);
/*
APP_START(qmp)
.init = qmp_init,
.entry = qmp,
.flags = 1,
APP_END
*/

APP_START(qmp)
APP_END
