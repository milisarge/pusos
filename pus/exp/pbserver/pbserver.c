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
#include <lib/pb_encode.h>
#include <lib/pb_decode.h>
#include "simple.pb.h"

#define LOCAL_TRACE 1

#define PBSERVER_PORT 8888

static int pbserver_worker(void *socket) {
    int i;
    int err;
    tcp_socket_t *sock = socket;

#define BUFSIZE 128
    /*
    uint8_t *readbuf = malloc(BUFSIZE);
    if (!readbuf) {
        LTRACEF("error allocating buffer\n");
        return ERR_NO_MEMORY;
    }
    */
    uint8_t buffer[BUFSIZE];
    size_t message_length;
    bool status;
    memset(buffer, 0xff, BUFSIZE);
	//for (;;) {
	  //thread_sleep(500);
	  err = tcp_read(sock, buffer, BUFSIZE);
	  if (err < 0) {
	    LTRACEF("error reading from socket:%d\n",err);
		return 1;
	  } 
	  // gelen cevabı okumak
	  else {
	    i = 0;
		while( i < sizeof(buffer )) {
		  printf("%02x ",buffer[i]);
		  if(buffer[++i] == 0)
		    break;
		}
		message_length = i;
		printf(":msglen:%d\n",message_length);
		printf("msg:%s\n","-");
		// parse req
		//parse_request(buffer, sock);  
		SimpleMessage message = SimpleMessage_init_zero;
        
        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
        
        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, SimpleMessage_fields, &message);
        
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
        
        /* Print the data contained in the message. */
        printf("Your lucky number was %d!\n", message.lucky_number);
		  
		//printf("..\n");
		/*
		err = tcp_write(sock, "GOT_IT\n", strlen("GOT_IT\n"));
		if (err < 0) {
		  LTRACEF("error writing to socket\n");
		  break;
		}
		* */
	  //}  // forloop
	}

    LTRACEF("pbserver worker exiting\n");
    tcp_close(sock);
    //free(buffer);

    return 0;
}

static void pb1_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;

    err = tcp_open_listen(&listen_socket, PBSERVER_PORT);
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

        LTRACEF("starting pbserver worker\n");
        thread_detach_and_resume(thread_create("Pus_API_Worker", &pbserver_worker, accept_socket, DEFAULT_PRIORITY, 8196));
    }
}

static void pbserver_entry(const struct app_descriptor *app, void *args) {
    thread_detach_and_resume(thread_create("PB1_Server", &pb1_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

APP_START(pbserver)
.init = NULL,//http_init,
.entry = pbserver_entry,
.flags = 0,
APP_END

