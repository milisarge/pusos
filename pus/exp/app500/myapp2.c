#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>

#include <lib/minip.h>
#include <lib/bio.h>
#include <lib/littlefs.h>
#include <rand.h>

#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
mutex_t pipe_lock_501;

#define BUFFER_SIZE 9
uint8_t buffer_501[BUFFER_SIZE*6];
uint8_t producer_item[6];

int out_x = 0;
int in_x = 0;
semaphore_t empty_sem;
semaphore_t full_sem;

void gen_rdm_bytestream_501 (size_t num_bytes) {
  size_t i;
  for (i = 0; i < num_bytes; i++) {
    producer_item[i] = rand ();
  }
}

// it gets 6byte char
int pub501(int id) {
    uint8_t poison[6] = {0,0,0,0,0,0};
    
    for (int i=0;i<20;i++) {
	  gen_rdm_bytestream_501(6);
      sem_timedwait(&empty_sem,5000);
      printf("sem-bekleme-bitti\n");
      mutex_acquire(&pipe_lock_501);
      for (int i = 0; i<6; i++){
        buffer_501[(in_x*6)+i] = producer_item[i];
	  }
      printf("pub%d-%d-",*((int *)id), i+1);
      for (int i=0;i<6;i++){printf("%x", producer_item[i]);}
      printf("\n");
      in_x = (in_x+1)%BUFFER_SIZE;
      printf("in_X:%d\n",in_x);
      mutex_release(&pipe_lock_501);
      //printf("pubmutex-relased%d\n",*((int *)id));
      sem_post(&full_sem, true);
      printf("fullsem_count%d\n",full_sem.count);
      thread_sleep(500);
      //printf("pubawaking%d\n",*((int *)id));
    }
	return 0;
}

int sub501(int id) {
    
    for (int i=0;i<22; i++) {
      printf("data-waiting%d\n",*((int *)id));
      sem_timedwait(&full_sem,3000);
      mutex_acquire(&pipe_lock_501);
      uint8_t item[6];
      for (int i = 0;i<6;i++){
        item[i] = buffer_501[(out_x*6)+i];
	  }
      printf("sub%d-%d-",*((int *)id),i+1);
      for (int i=0;i<6;i++){printf("%x", item[i]);}
      printf("\n");
      out_x = (out_x+1)%BUFFER_SIZE;
      printf("out_X:%d\n",out_x);
      mutex_release(&pipe_lock_501);
      //printf("submutex-relased%d\n",*((int *)id));
      sem_post(&empty_sem, true);
      
      //printf("subsem-relased%d\n",*((int *)id));
      thread_sleep(500);
	}
	return 0;
}
/*
int read_sensor_data_501() {
	lfs_file_t file;
    // block device açma
    //LTRACEF("opening the block device\n");
    lfs_dev = bio_open(lfs_dev_name);
    if (!lfs_dev) { return -1; }
    int err = lfs_mount(&lfs, &lfs_cfg);
    if (err) {
        lfs_format(&lfs, &lfs_cfg);
        lfs_mount(&lfs, &lfs_cfg);
    }
    
    // read current count
    float temp_value = 0.0;
    char x;
    lfs_file_open(&lfs, &file, "sensor_data", LFS_O_RDWR | LFS_O_CREAT);
    for (int i=0;i<100; i++) {
		thread_sleep(1000);
		lfs_file_read(&lfs, &file, &temp_value, sizeof(temp_value));
		//printf("%d- %f\n", i+1, temp_value);
		lfs_file_seek(&lfs, &file, 1, LFS_SEEK_CUR);
		rule_low_temp(temp_value);
		rule_high_temp(temp_value);
		if (! lfs_file_available(&lfs, &file) || temp_value == 0.0) {
			break;
		}
	}
    lfs_file_close(&lfs, &file);
    return 0;
}
*/
static int app501(int argc, const console_cmd_args *argv) {
    srand(current_time());
	thread_t pro[1],con[3];
    mutex_init(&pipe_lock_501);
    sem_init(&empty_sem, BUFFER_SIZE);
    sem_init(&full_sem, 0);
    
    int a[3] = {1,2,3}; //Just used for numbering the producer and consumer

    for(int i = 0; i < 1; i++) {
        //pro[i] = thread_create("501-pub", &pub501, (void *)&a[i], DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
        thread_create_etc(&pro[i], "501-pub", &pub501, (void *)&a[i], DEFAULT_PRIORITY, NULL, DEFAULT_STACK_SIZE) ;
        thread_resume(&pro[i]);
    }
    for(int i = 0; i < 3; i++) {
        //con[i] = thread_create("501-sub", &sub501, (void *)&a[i], DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
        thread_create_etc(&con[i], "501-sub", &sub501, (void *)&a[i], DEFAULT_PRIORITY, NULL, DEFAULT_STACK_SIZE) ;
        thread_resume(&con[i]);
    }
    
     for(int i = 0; i < 1; i++) {
        thread_join(&pro[i], NULL, INFINITE_TIME);
        printf("pub-join\n");
    }
    for(int i = 0; i < 3; i++) {
        thread_join(&con[i], NULL, INFINITE_TIME);
        printf("sub%d-join\n",i+1);
    }
    
	mutex_destroy(&pipe_lock_501);
    sem_destroy(&empty_sem);
    sem_destroy(&full_sem);
    
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app501", "app501 tests", &app501)
STATIC_COMMAND_END(app501);

APP_START(app501)
APP_END

