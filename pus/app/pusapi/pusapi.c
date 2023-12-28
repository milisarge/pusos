#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>
#include <lk/err.h>
#include <lib/minip.h>
#include <malloc.h>
#include <lk/trace.h>

#include <lib/llhttp.h>

#include <lib/binn.h>

#include <lib/bio.h>

#define LOCAL_TRACE 0

#define PUSAPI_PORT 4999

#define MAX_LEN 1024

#define LOCAL_TRACE 1

typedef struct {
  uint16_t message_id; //2b
  //lk_time_t c_time;  
  uint8_t type; // 1b  options 0a:temp, 0b:humidity
  float value; // 4b	
} sensor_data_t; //8b

// http struct

#define RESPONSE "hello_world"
struct http_string_s {
  char const * buf;
  int len;
};

// Returns the current chunk of the request body. This chunk is only valid until
// the next call to `http_request_read_chunk`.
struct http_string_s http_request_chunk(struct http_request_s* request);


struct http_server_s;
struct http_request_s;
struct http_response_s;


typedef struct http_header_s {
  char const * key;
  char const * value;
  struct http_header_s* next;
} http_header_t;

typedef struct http_response_s {
  http_header_t* headers;
  char const * body;
  int content_length;
  int status;
} http_response_t;

typedef struct http_string_s http_string_t;

http_response_t* http_response_init() {
  http_response_t* response = (http_response_t*)calloc(1, sizeof(http_response_t));
  assert(response != NULL);
  response->status = 200;
  return response;
}

void http_response_header(http_response_t* response, char const * key, char const * value) {
  http_header_t* header = (http_header_t*)malloc(sizeof(http_header_t));
  assert(header != NULL);
  header->key = key;
  header->value = value;
  http_header_t* prev = response->headers;
  header->next = prev;
  response->headers = header;
}

void http_response_status(http_response_t* response, int status) {
  response->status = status > 599 || status < 100 ? 500 : status;
}

void http_response_body(http_response_t* response, char const * body, int length) {
  response->body = body;
  response->content_length = length;
}

// routes
#define API "/api"
#define INFO "/info"
#define DATA "/data"

llhttp_t parser;
llhttp_settings_t settings;

char *reply;
char buffer[1200];

// örnek cevap
        char *reply0 = 
"HTTP/1.1 200 OK\n"
"Date: Fri, 23 Sep 2022 21:27:04 GMT\n"
"Server: LK/0.0.1\n"
"Last-Modified: Wed, 18 Jun 2023 16:05:58 GMT\n"
"ETag: \"56d-9989200-1132c580\"\n"
"Content-Type: application/json\n"
"Content-Length: 16\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
"\n"
"{\"data\":\"hello world from PusOS\"}";

        char *reply1 = 
"HTTP/1.1 200 OK\n"
"Date: Fri, 23 Sep 2022 21:27:04 GMT\n"
"Server: LK/0.0.1\n"
"Last-Modified: Wed, 18 Jun 2023 16:05:58 GMT\n"
"ETag: \"56d-9989200-1132c580\"\n"
"Content-Type: application/json\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
"Content-Length: 16\n"
"\n"
"{\"data\":\"hello world from PusOS\"}";

        char *reply2 = 
"HTTP/1.1 200 OK\n"
"Date: Fri, 23 Sep 2022 21:27:04 GMT\n"
"Server: LK/0.0.1\n"
"Last-Modified: Wed, 18 Jun 2023 16:05:58 GMT\n"
"ETag: \"56d-9989200-1132c580\"\n"
"Content-Type: text/html\n"
"Content-Length: 35\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
"\n"
"<html><h3>hello world from PusOS</h3></html>";

        char *reply3 = 
"HTTP/1.1 404 Not Found\n"
"Content-Type: text/html\n"
"Content-Length: 120\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
"\n"
"<html><head><title>404 Not Found</title></head><body bgcolor=white><center><h1>404 Not Found</h1></center></body></html>";

int get_sensor_data(char *buffer) {
   int bfw = 0;
   const char *reply_bas = 
"HTTP/1.1 200 OK\n"
"Server: LK/0.0.1\n"
"Content-Type: text/html\n"
"Accept-Ranges: bytes\n";
    char body[700];
    memset(body,0,700);
    memcpy(buffer+bfw, reply_bas, strlen(reply_bas));
    bfw += strlen(reply_bas);
    const char * dev_name="virtio0";
    bdev_t *dev = bio_open(dev_name);
    if (!dev) {
        return -1;
    }
    ssize_t buflen = 16;//sizeof(sensor_data_t);
	ssize_t writelen = 0;
    uint8_t data_count = 9;
    // 9 gönderemiyor
    
    void *ptr = memalign(0, buflen * data_count);
    ssize_t readlen = bio_read(dev, ptr, 0, buflen * data_count);
    
    printf("----------------------------\n");
    uint8_t* dptr = (uint8_t*)ptr;
    //sensor verileri okuma
    sensor_data_t datap;
    char aras[128];
    memset(aras,0,128);
    for (int i =1; i < data_count +1; i++) {
	  datap.message_id = dptr[0] + (dptr[1] << 8);
	  datap.type = dptr[2] ;
	  uint8_t bytes[4] = { dptr[4], dptr[5],dptr[6], dptr[7] };
	  memcpy (&datap.value, bytes, 4);
	  //printf("block_read: time:%d msg:%02x type:%x temp(C):%f\n",(dptr[8]), datap.message_id, datap.type, datap.value);
	  //sprintf(aras, "block_read: time:%d msg:%02x type:%x temp(C):%f<br>",(dptr[8]), datap.message_id, datap.type, datap.value);
	  sprintf(aras, "blockid: time:%d msg:%02x type:%x temp(C):%f<br>",(dptr[8]), datap.message_id, datap.type, datap.value);
	  strcat(body,aras);
	  memset(aras,0,128);
	  dptr = dptr + buflen;
    }
    memset(aras,0,128);
    sprintf(aras, "Content-Length: %d\n", strlen(body));
    memcpy(buffer+bfw, aras, strlen(aras));
    bfw += strlen(aras);
    
    memset(aras,0,128);
    sprintf(aras, "%s", "Connection: close\n\n");
    memcpy(buffer+bfw, aras, strlen(aras));
    bfw += strlen(aras);
    
    memcpy(buffer+bfw, body, strlen(body));
    bfw += strlen(body);
    
    buffer[bfw] = '\0';
    printf("httppaketboyut:%d\n",strlen(buffer));
    bio_close(dev);
    return 0;
}

int on_message_begin(llhttp_t* parser)
{
    printf("parse start\n");
    return 0;
}

int on_url(llhttp_t* parser, const char* at, size_t length)
{
    char url[MAX_LEN];
    strncpy(url, at, length);
    url[length] = '\0';
    printf("on_url: %s\n", url);
    if (strcmp(API,url) == 0) {
      reply = reply1;
    } else if (strcmp(INFO,url) == 0) {
	  //reply = reply2;
	  struct http_response_s* response = http_response_init();
      http_response_status(response, 200);
      //http_string_t body = http_request_body(request);
      http_response_header(response, "Content-Type", "text/plain");
      http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
	} else if (strcmp(DATA,url) == 0) {
      
	  get_sensor_data(&buffer);
	  printf("donen:%d:\n",strlen(buffer));
	  reply = &buffer;
	  
	} else {
	  reply = reply3;
    }
    return 0;
}

int on_header_field(llhttp_t* parser, const char* at, size_t length)
{
    char header_field[MAX_LEN];
    strncpy(header_field, at, length);
    header_field[length] = '\0';
    printf("head field: %s\n", header_field);
    return 0;
}

int on_header_value(llhttp_t* parser, const char* at, size_t length)
{
    char header_value[MAX_LEN];
    strncpy(header_value, at, length);
    header_value[length] = '\0';
    printf("head value: %s\n", header_value);
    return 0;
}

int on_headers_complete(llhttp_t* parser)
{
    printf("on_headers_complete, major: %d, major: %d, keep-alive: %d, upgrade: %d\n", parser->http_major, parser->http_minor, llhttp_should_keep_alive(parser), parser->upgrade);
    return 0;
}

int on_body(llhttp_t* parser, const char* at, size_t length)
{
    char body[MAX_LEN];
    strncpy(body, at, length);
    body[length] = '\0';
    printf("on_body: %s\n", body);
    return 0;
}

int on_message_complete(llhttp_t* parser)
{
    printf("on_message_complete\n");

    return 0;
}

static void parse_request(const char *request, tcp_socket_t *socket) {
	/* Initialize user callbacks and settings */
	llhttp_settings_init(&settings);
	//llhttp_init(&parser, HTTP_REQUEST, &settings);

	/* Set user callback */
	settings.on_message_begin = on_message_begin;
    settings.on_url = on_url;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_headers_complete = on_headers_complete;
    settings.on_body = on_body;
    settings.on_message_complete = on_message_complete;

	/* Initialize the parser in HTTP_BOTH mode, meaning that it will select between
	 * HTTP_REQUEST and HTTP_RESPONSE parsing automatically while reading the first
	 * input.
	 */
	llhttp_init(&parser, HTTP_BOTH, &settings);

	/* Parse request! */
	//const char* request = "POST / HTTP/1.1\r\n\r\n";
	int request_len = strlen(request);

    llhttp_execute(&parser, request, request_len);
    
    printf("resp:%s:%d\n",reply,strlen(reply));
    int err = tcp_write(socket, reply, strlen(reply));
    if (err < 0) {
	  LTRACEF("error writing to socket\n");
    }
    /*
    enum llhttp_errno err = llhttp_execute(&parser, request, request_len);
    if (err != HPE_OK) {
       fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err),
            parser.reason);
    }
    */
}

static int pusapi_worker(void *socket) {
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

    LTRACEF("pusapi worker exiting\n");
    tcp_close(sock);
    free(readbuf);

    return 0;
}

static void pusapi_server(void *arg) {
    status_t err;
    tcp_socket_t *listen_socket;

    err = tcp_open_listen(&listen_socket, PUSAPI_PORT);
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

        LTRACEF("starting pusapi worker\n");
        thread_detach_and_resume(thread_create("Pus_API_Worker", &pusapi_worker, accept_socket, DEFAULT_PRIORITY, 8196));
    }
}

static void pusapi_entry(const struct app_descriptor *app, void *args) {
    thread_detach_and_resume(thread_create("Pus_API_Server", &pusapi_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
}

APP_START(pusapi)
.init = NULL,//pusapi_init,
.entry = pusapi_entry,
.flags = 0,
APP_END
