/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <lib/bio.h>
#include <lk/trace.h>


#define LOCAL_TRACE 1

typedef struct {
  uint16_t message_id; //2b
  //lk_time_t c_time;  
  uint8_t type; // 1b  options 0a:temp, 0b:humidity
  float value; // 4b	
} sensor_data_t; //8b

/*
ssize_t fill_buffer (sensor_data_t data,  uint8_t **buffer) {
	
	*buffer = malloc((data.len+2) * sizeof(uint8_t));
	(*buffer)[0] = data.message_id;
	(*buffer)[1] = data.len;
	for (int i=2; i<data.len+2; i++) {
      (*buffer)[i] = *data.value++;
    }
    return data.len+2;
}
*/

static int datastore(int argc, const console_cmd_args *argv) {
    LTRACEF("opening the block device\n");
    const char * dev_name="virtio0";
    bdev_t *dev = bio_open(dev_name);
    if (!dev) {
        return -1;
    }

    ssize_t buflen = 16;//sizeof(sensor_data_t);
	ssize_t writelen = 0;
    uint8_t data_count = 10;
    
    /*
    // sıfırlama
    char temp_buffer[1024] = { 0 };
    bio_write(dev, &temp_buffer, 0, buflen * data_count);
    // veri üretme ve yazma
    for (int i =1; i < data_count + 1; i++) {
	  sensor_data_t datax = { i, 0x0a, (i * 10) + 1 };
      writelen += bio_write(dev, &datax, writelen, buflen);
	  printf("block_write: msg:%d type:%x temp(C):%f\n",datax.message_id, datax.type, datax.value);
    }
    */
    // boundry, size
    void *ptr = memalign(0, buflen * data_count);
    //void *ptr = malloc(dev->block_size);
    LTRACEF("mem address:%p\n",ptr);
    ssize_t readlen = bio_read(dev, ptr, 0, buflen * data_count);
    //LTRACEF("read byte len: %ld\n", readlen);
    
    printf("----------------------------\n");
    uint8_t* dptr = (uint8_t*)ptr;
    //sensor verileri okuma
    sensor_data_t datap;
    
    for (int i =1; i < data_count +1; i++) {
	  datap.message_id = dptr[0] + (dptr[1] << 8);
	  datap.type = dptr[2] ;
	  uint8_t bytes[4] = { dptr[4], dptr[5],dptr[6], dptr[7] };
	  memcpy (&datap.value, bytes, 4);
	  printf("block_read: time:%d msg:%02x type:%x temp(C):%f\n",(dptr[8]), datap.message_id, datap.type, datap.value);
	  dptr = dptr + buflen;
    }
    
	/*
    void *ptr2 = memalign(0,30);
    //void *ptr = malloc(dev->block_size);
    LTRACEF("mem address:%p\n",ptr2);
    ssize_t readlen2 = bio_read(dev, ptr2, 0, 30);
    LTRACEF("read byte len: %ld\n", readlen2);
    */
    
    bio_close(dev);
    LTRACEF("block device closed\n");
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("datastore", "datastore tests", &datastore)
STATIC_COMMAND_END(datastore);

APP_START(datastore)
APP_END

