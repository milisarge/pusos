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
#include "raft.pb.h"

#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"

//#include <lib/setjmp.h>

#include "app_raft.h"
mutex_t mu_lock_raft1;

mutex_t hb_wait_lock;

mu_t scope_raft1;

#define LOCAL_TRACE 1

#define RAFTSERVER_PORT 8811

bool print_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buffer[1024] = {0};
    
    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;
    
    /* Print the string, in format comparable with protoc --decode.
     * Format comes from the arg defined in main().
     */
    printf((char*)*arg, buffer);
    return true;
}

bool get_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
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

static int raftserver_worker(void *socket) {
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
		AppendEntriesRequest message = AppendEntriesRequest_init_zero;
        
        message.id.funcs.decode = &get_string;
        message.entries.funcs.decode = &get_string;
        //message.id.funcs.decode = &print_string;
        //message.id.arg = ": %s\n";
        char msg_id[30];
        char entries[30];
        message.id.arg = &msg_id;
        message.entries.arg = &entries;
        
        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
        
        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, AppendEntriesRequest_fields, &message);
        
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
        /*
		string id = 1;              // The uuid of the Raft node sending the request.(leader)
		int32 term = 2;             // The term from the requester's perspective.
		int32 prevLogIndex = 4;     // Index of entry immediately preceding new ones.
		int32 prevLogTerm = 5;      // The term of the prevLogIndex entry.
		repeated string entries = 6; // New ops to append.
		int32 leaderCommit = 7;     // The leader’s commitIndex. - what index have been received by the majority
        */
        
        /* Print the data contained in the message. */
        //char leader[sizeof(message.id)];
        //strncpy(leader, message.id, sizeof(message.id));
        printf("id:%s\n",msg_id);
        
        printf("term:%d\n", message.term);
        printf("prevLogIndex:%d\n", message.prevLogIndex);
        printf("prevLogTerm:%d\n", message.prevLogTerm);
        printf("entries:%s\n", entries);
        printf("leaderCommit:%d\n", message.leaderCommit);
	    
	    
	    mutex_acquire_timeout(&mu_lock_raft1, 2500);
		// iterativ yazdırma any ile
		//char *cmd = "print(temp);print(humidity)";
		if (mu_isnil(scope_raft1)){
			scope_raft1 = mu_tbl_extend(0, MU_BUILTINS);
		}
		mu_t ret;
		if (!setjmp(mu_error_jmp)) {
			ret = mu_eval(entries, strlen(entries), scope_raft1, 0);
		}
		// eğer wm bozulursa geri dönüş değerinden tekrar scope oluşturulup eşlenecek
		//printf("ret:%d\n",ret);
		if (ret == 0) {
			printf("command is successful\n");
		} else {
			printf("command is failed\n");
		}
		mutex_release(&mu_lock_raft1);
		printf("release mux\n");
		/*
		err = tcp_write(sock, "GOT_IT\n", strlen("GOT_IT\n"));
		if (err < 0) {
		  LTRACEF("error writing to socket\n");
		  break;
		}
		* */
	  //}  // forloop
	}

    LTRACEF("raftserver worker exiting\n");
    tcp_close(sock);
    //free(buffer);

    return 0;
}

static bool write_repeated_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, 0, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, *arg, strlen(*arg));
}

bool set_raftrequest_data(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{   
    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    return pb_encode_string(stream, (uint8_t*)*arg, strlen(*arg));
}

int raft_sender(char *cmd, int term_arg) {
    uint32_t server;
    uint16_t port;
    status_t err;
    server = IPV4(192,168,122,1);
    port = 8811;
    udp_socket_t *sock ;
    uint8_t buffer[128];
    unsigned short i;
    
    //thread_sleep((rand() % 5) * 1000);
    //hread_sleep(5000);
    //for (;;) { 
		//printf("t1----------------\n");
	while(!minip_is_configured()){
	  thread_sleep(2000);
	}
	//err = tcp_connect(&sock, server, port);
	err = udp_open(server, port, port, &sock);
	if (err < 0) {
	  LTRACEF("error connecting to socket\n");
	  //return -1;
	}
	
	AppendEntriesRequest request = AppendEntriesRequest_init_zero;
	memset(buffer, 0xff, 128);
	pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	
	request.id.funcs.encode = &set_raftrequest_data;
	request.id.arg = "pus1";
	request.entries.funcs.encode = &set_raftrequest_data;
	request.entries.arg = cmd;
	request.term = term_arg;
	request.prevLogIndex = 0;
	request.prevLogTerm = 0;
	request.leaderCommit = 0;
	
	status_t status = pb_encode(&ostream, AppendEntriesRequest_fields, &request);
	size_t request_length = ostream.bytes_written;
	
	if (!status)
	{
		printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
		return 1;
	}
	
	//err = tcp_write(sock, buffer, request_length);
	err = udp_send(buffer, request_length, sock);
	if (err < 0) {
	  LTRACEF("error writing to socket\n");
	  //return 1;
	}
	
	// close seocket
	//tcp_close(sock);
	//thread_sleep(1200);
	//thread_sleep((rand() % 3) * 1000);
	//mutex_acquire_timeout(&hb_wait_lock, 1000);
	//mutex_release(&hb_wait_lock);
	//printf("hb wait ended\n");
	udp_close(sock);
	 //   break;
    //}

    return 0;
}

static int raft_worker(void *arg) {
    uint32_t server;
    uint16_t port;
    status_t err;
    server = IPV4(192,168,122,1);
    port = 8811;
    udp_socket_t *sock ;
    uint8_t buffer[128];
    unsigned short i;
    
    //thread_sleep((rand() % 5) * 1000);
    //hread_sleep(5000);
    for (;;) { 
		//printf("t1----------------\n");
        
        while(!minip_is_configured()){
		  thread_sleep(2000);
		}
        //err = tcp_connect(&sock, server, port);
        err = udp_open(server, port, port, &sock);
        if (err < 0) {
          LTRACEF("error connecting to socket\n");
          //return -1;
        }
		
		AppendEntriesRequest request = AppendEntriesRequest_init_zero;
		memset(buffer, 0xff, 128);
		pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
		
		request.id.funcs.encode = &set_raftrequest_data;
		request.id.arg = "pus1";
		request.entries.funcs.encode = &set_raftrequest_data;
		request.entries.arg = "heartbeat";
		request.term = 1000;
		request.prevLogIndex = 0;
		request.prevLogTerm = 0;
		request.leaderCommit = 0;
		
        status_t status = pb_encode(&ostream, AppendEntriesRequest_fields, &request);
        size_t request_length = ostream.bytes_written;
        
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }
		
		//err = tcp_write(sock, buffer, request_length);
		err = udp_send(buffer, request_length, sock);
		if (err < 0) {
		  LTRACEF("error writing to socket\n");
		  //return 1;
		}
		
        // close seocket
        //tcp_close(sock);
        thread_sleep(1200);
        //thread_sleep((rand() % 3) * 1000);
        //mutex_acquire_timeout(&hb_wait_lock, 1000);
        //mutex_release(&hb_wait_lock);
	    //printf("hb wait ended\n");
	    udp_close(sock);
	    //thread_sleep((rand() % 4) * 1000);
    }

    return 0;
}


static void pb1_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;
	if (mu_isnil(scope_raft1)){
		scope_raft1 = mu_tbl_extend(0, MU_BUILTINS);
	}
	
	mutex_init(&mu_lock_raft1);
	
	mutex_init(&hb_wait_lock);
	
    err = tcp_open_listen(&listen_socket, RAFTSERVER_PORT);
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

        LTRACEF("starting raftserver worker\n");
        thread_detach_and_resume(thread_create("Raft_Worker", &raftserver_worker, accept_socket, DEFAULT_PRIORITY, 8196));
    }
}

static void raftserver_entry(const struct app_descriptor *app, void *args) {
    thread_detach_and_resume(thread_create("Raft1_Server", &pb1_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    thread_detach_and_resume(thread_create("Raft1_Worker", &raft_worker, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

APP_START(raftserver)
.init = NULL,//http_init,
.entry = raftserver_entry,
.flags = 0,
APP_END

