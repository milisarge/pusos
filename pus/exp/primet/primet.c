#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

typedef struct{
  int id;
  int st;
  int fn;
  int result;
}prime_arg;

// asal tespiti
int is_prime (int n){
  //for (int i=2;i<sqrt(n)+1;i++){
  for (int i=2;i<n/2;i++){
    if (n % i == 0) return 0;
  }
  return 1;
}

void *primet_worker (void *param){
  int i=0;
  prime_arg *parg= param;
  
  for (int i = parg->st; i < (parg->fn); i++) {
    is_prime(i) ? printf("t%d:----%d\n", parg->id, i) : 0;
  }
  //parg->result=parg->id*10;
  return NULL;
}

static int primet (int argc, const console_cmd_args *argv) {
  int tnum=argv[1].i;
  thread_t *t[tnum];
  int i=0;
  prime_arg argp[tnum];
  int ia[]={0,50,100,150,200,250};
  while (i<tnum){
    argp[i].id = i ;
    argp[i].st = ia[i];
    argp[i].fn = ia[i+1];
    //thread_detach_and_resume(thread_create("primet_worker", (void *)&primet_worker, &argp[i], DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
    t[i]=thread_create("primet_worker", (void *)&primet_worker, &argp[i], DEFAULT_PRIORITY, DEFAULT_STACK_SIZE); 
    //thread_resume(t[i]);
    i++;
  }
  /*
  i--;
  while(i>-1){
    thread_resume(t[i]);
    thread_join(t[i], NULL, INFINITE_TIME);
    //thread_detach_and_resume(t[i]);
    i--;
  }
  */
  i=0;while(i<tnum){ thread_resume(t[i]);i++;}
  i=0;while(i<tnum){ thread_join(t[i], NULL, INFINITE_TIME);i++;}
  //i=0;while(i<tnum){ thread_resume(t[i]);thread_join(t[i], NULL, INFINITE_TIME);i++;}

  
  
  return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("primet", "multithread prime numbers tests", &primet)
STATIC_COMMAND_END(primet);

APP_START(primet)
APP_END

