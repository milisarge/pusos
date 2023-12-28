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
#include <lib/console.h>

#include <lib/pb_encode.h>
#include <lib/pb_decode.h>
#include "cmdserver.pb.h"

#define LOCAL_TRACE 0

#define cmdserver_PORT 5000

bool get_cmdserver_data(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buffer[1024] = {0};
    
    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;
        
    memcpy(*arg, buffer, 30);
    return true;
}

bool set_cmdserver_data(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{   
    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    return pb_encode_string(stream, (uint8_t*)*arg, strlen(*arg));
}


static int cmdserver_worker(void *socket) {
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

	  err = tcp_read(sock, buffer, BUFSIZE);
	  if (err < 0) {
	    LTRACEF("error reading from socket:%d\n",err);
		return 1;
	  } 
	  // gelen cevabı okumak
	  else {
	    i = 0;
		while( i < sizeof(buffer )) {
		  //printf("%02x ",buffer[i]);
		  if(buffer[++i] == 0)
		    break;
		}
		size_t request_length = i;

		CmdRequest request= CmdRequest_init_zero;
        
        request.fn.funcs.decode = &get_cmdserver_data;
        char fn_name[30];
        request.fn.arg = &fn_name;
        
        request.data.funcs.decode = &get_cmdserver_data;
        char request_data[30];
        request.data.arg = &request_data;
        
        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, request_length);
        
        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, CmdRequest_fields, &request);
        
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }

        //printf("id:%d\n",request.id);
        printf("\nfn:%s\n",fn_name);
        //printf("data:%s\n",request_data);
        
        CmdResponse response = CmdResponse_init_default;
        // run script from fn_name
        console_t *con = console_create(false);
		int cmd_result = console_run_script(con, fn_name);	
        if (cmd_result == 0){
			response.data.arg = "remote execution is successfull!";
		} else {
			response.data.arg = "remote execution is failed!";	
		}
		// cmdreponse döndür
		memset(buffer, 0xff, BUFSIZE);
		pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
		
		response.data.funcs.encode = &set_cmdserver_data;
		response.id = request.id ;
		
        status = pb_encode(&ostream, CmdResponse_fields, &response);
        size_t response_length = ostream.bytes_written;
        
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }
		
		err = tcp_write(sock, buffer, response_length);
		if (err < 0) {
		  LTRACEF("error writing to socket\n");
		  return 1;
		}
		
	  }

    LTRACEF("cmdserver worker exiting\n");
    tcp_close(sock);
    //free(buffer);

    return 0;
}

static void cmd_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;
	
	
    err = tcp_open_listen(&listen_socket, cmdserver_PORT);
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

        LTRACEF("starting cmdserver worker\n");
        thread_detach_and_resume(thread_create("CmdServer_Worker", &cmdserver_worker, accept_socket, DEFAULT_PRIORITY, 8196));
    }
}

static void cmdserver_entry(const struct app_descriptor *app, void *args) {
    thread_detach_and_resume(thread_create("Cmd_Server", &cmd_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

APP_START(cmdserver)
.init = NULL,//http_init,
.entry = cmdserver_entry,
.flags = 0,
APP_END

