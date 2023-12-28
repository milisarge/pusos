#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>

#include <lib/minip.h>

#define LOCALH IPV4(192,168,122,99)

static void pubsub(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
  printf("---");
  printf("data:%s\n",data);
  // yanıt
  udp_socket_t *socket;
  status_t st;
  if (udp_open(srcaddr, 444, srcport, &socket) != 0) {
		printf("udp_open to %u.%u.%u.%u:%u failed\n", IPV4_SPLIT(srcaddr), srcport);
		return ;
  }
  st = udp_send("alindi", 6, socket);
}

int pubsub_service() {
  udp_listen(444, (void *)&pubsub, 0);
  return 0;
}

static void rule1_service_handle(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
  printf("---");
  printf("rule1yanit:%s\n",data);
}

int rule1_service(char * param) {
    
    for (int i=0;i<5;i++){
		//printf("%p - %d - %s.\n",get_current_thread(), i,param);
		printf("%d-%s\n",i,param);
		thread_sleep(1000);
	}
	udp_listen(441, (void *)&rule1_service_handle, 0);
	udp_socket_t *socket;
	status_t st;
	printf("veri gonderiliyor..\n");
	if (udp_open(0, 441, 444, &socket) != 0) {
		printf("udp_open to failed\n");
		return -1;
	}
	st = udp_send("rule1tamam", 10, socket);
	printf("sonuc:%d\n",st);
	/*
	static uint8_t xyz = 254;
	printf("xyz- %p\n",&xyz);
	uint8_t *stack_base;
    size_t stack_size;
    stack_base = get_current_thread()->stack;
    stack_size = get_current_thread()->stack_size;
	
	for (int i = 0; i < stack_size; i++) {
        if (stack_base[i] == (0x99) )
            continue;
        printf("%x",stack_base[i]);
    }
    */
	//printf("rule1 stck: %b\n",);
	return 0;
}
int rule2_service(char * param) {
  
    for (int i=10;i<13;i++){
		printf("%p - %d - %s.\n",get_current_thread(), i,param);
		thread_sleep(1000);
		//thread_yield();
	}
	return 0;
}

static int app400(int argc, const console_cmd_args *argv) {
    /*
    for (int i=0;i<2;i++){
		thread_sleep(1000);
		printf("%d - bu bir test uygulamasıdır.\n",i);
	}
	*/
    //const char * mystr = "This string is definitely longer than what we want to print.";
    //printf("Here are first 5 chars only: %.5s\n", mystr);
    //printf("%.*s\n", 5, mystr);
    thread_t *pubsub_thread = thread_create("pubsub", &pubsub_service, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(pubsub_thread);
    
    char * param = "deneme";
    thread_t *my_thread = thread_create("rule1", &rule1_service, param, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(my_thread);
    thread_t *my_thread2; 
    //my_thread2 = thread_create("rule2", &rule2_service, param, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    my_thread2 = thread_create("rule2", &rule2_service, param, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(my_thread2);
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app400", "app400 tests", &app400)
STATIC_COMMAND_END(app400);

APP_START(app400)
APP_END

