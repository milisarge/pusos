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
mutex_t pipe_lock;

lfs_t lfs;

unsigned char *gen_rdm_bytestream (size_t num_bytes) {
  unsigned char *stream = malloc (num_bytes);
  size_t i;
  
  for (i = 0; i < num_bytes; i++)
  {
    stream[i] = rand ();
  }
  return stream;
}

// it gets 6byte char
int pub500() {
    int st;  
    lfs_file_t file;
    // write value
    uint8_t *param;
    srand(current_time());
    for (int i=0;i<10;i++) {
	  //printf("turp:%d\”n",i);
	  mutex_acquire(&pipe_lock);
	  //st = lfs_file_open(&lfs, &file, "pipe0", LFS_O_RDWR | LFS_O_CREAT);
	  st = lfs_file_open(&lfs, &file, "pipe0", LFS_O_RDWR | LFS_O_CREAT);
	  //printf("popen_pub:%d\n",st);
      if (st != 0) {
	    mutex_release(&pipe_lock);
	    return 1; 
	  }
      //lfs_file_rewind(&lfs, &file);
      param = gen_rdm_bytestream(6);
      //printf("u:%#X\n",param);
      lfs_file_write(&lfs, &file, param, 6);
      lfs_file_write(&lfs, &file, "\n", 1);
      printf("pub-%d-",i+1);
      for (int i=0;i<6;i++){printf("%x", param[i]);}
      printf("\n");
      lfs_file_close(&lfs, &file);
      mutex_release(&pipe_lock);
      thread_sleep(1000);
    }
    // remember the storage is not updated until the file is closed successfully
    //lfs_file_close(&lfs, &file);
	return 0;
}

int sub500() {
	int st;   
    uint8_t data[6];
    lfs_file_t file;
    
    for (int i=0;i<12; i++) {
		mutex_acquire(&pipe_lock);
		st = lfs_file_open(&lfs, &file, "pipe0", LFS_O_RDONLY);
		//printf("popen_sub:%d\n",st);
		if (st != 0) {
		  mutex_release(&pipe_lock);
		  return 1; 
		}
		lfs_file_read(&lfs, &file, &data, 6);
		printf("sub-%d-", i+1);
		for (int i=0;i<sizeof(data);i++){printf("%x", data[i]);}
		printf("\n");

		lfs_file_seek(&lfs, &file, 1, LFS_SEEK_CUR);
		// if pipe is empty then exit
		if (! lfs_file_available(&lfs, &file) ) {
			break;
		}
		lfs_file_close(&lfs, &file);
		mutex_release(&pipe_lock);
		thread_sleep(1000);
	}
    //lfs_file_close(&lfs, &file);
	return 0;
}

int sub500_2() {
	int st;   
    uint8_t data[6];
    lfs_file_t file;
    
    for (int i=0;i<11; i++) {
		mutex_acquire(&pipe_lock);
		st = lfs_file_open(&lfs, &file, "pipe0", LFS_O_RDONLY);
		//printf("popen_sub:%d\n",st);
		if (st != 0) {
		  mutex_release(&pipe_lock);
		  return 1; 
		}
		lfs_file_read(&lfs, &file, &data, 6);
		printf("sub2-%d-", i+1);
		for (int i=0;i<sizeof(data);i++){printf("%x", data[i]);}
		printf("\n");

		lfs_file_seek(&lfs, &file, 1, LFS_SEEK_CUR);
		// if pipe is empty then exit
		if (! lfs_file_available(&lfs, &file) ) {
			break;
		}
		lfs_file_close(&lfs, &file);
		mutex_release(&pipe_lock);
		thread_sleep(1000);
	}
    //lfs_file_close(&lfs, &file);
	return 0;
}


int rule_high_temp(float temp){
	if (temp > 50) {
	  printf("high temp:%f\n",temp);
	  return 1;
    }
	return 0;
}

int rule_low_temp(float temp){
	if (temp < 50) {
	  printf("low temp:%f\n",temp);
	  return 1;
    }
	return 0;
}

int read_sensor_data() {
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

static int app500(int argc, const console_cmd_args *argv) {
    /*
    for (int i=0;i<2;i++){
		thread_sleep(1000);
		printf("%d - bu bir test uygulamasıdır.\n",i);
	}
	*/
    //const char * mystr = "This string is definitely longer than what we want to print.";
    //printf("Here are first 5 chars only: %.5s\n", mystr);
    //printf("%.*s\n", 5, mystr);
    // block device açma
    //LTRACEF("opening the block device\n");
    lfs_dev = bio_open(lfs_dev_name);
    if (!lfs_dev) { return -1; }
    int err = lfs_mount(&lfs, &lfs_cfg);
    if (err) {
        lfs_format(&lfs, &lfs_cfg);
        lfs_mount(&lfs, &lfs_cfg);
    }
    
    //char * param = "d2n3mX";
    mutex_init(&pipe_lock);
    thread_t *my_thread = thread_create("500-pub", &pub500, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(my_thread);
    thread_t *my_thread2 = thread_create("500-sub", &sub500, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(my_thread2);
    thread_t *my_thread3 = thread_create("500-sub2", &sub500_2, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    thread_resume(my_thread3);
    /*
    for (int i=0;i<10;i++){
		rule_low_temp(i);
		rule_high_temp(i);
	}
	*/
	thread_join(my_thread, NULL, INFINITE_TIME);
	thread_join(my_thread2, NULL, INFINITE_TIME);
	thread_join(my_thread3, NULL, INFINITE_TIME);
	mutex_destroy(&pipe_lock);
	//sensor veri okuma degerlendirme
	//thread_t *se_thread = thread_create("evaluate_sensor_data", &read_sensor_data, param, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    //thread_resume(se_thread);
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app500", "app500 tests", &app500)
STATIC_COMMAND_END(app500);

APP_START(app500)
APP_END

