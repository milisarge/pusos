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
#include <lk/err.h>

#include <lib/minip.h>
#include <lk/trace.h>

// coap lib
#include <lib/coap.h>
// bio device ops
#include <lib/bio.h>
//generate msg ids
#include <rand.h>
#include <platform/rtc_time.h>
#include <lib/datetime.h>

#define LOCAL_TRACE 0
#define UDP_PORT 5683
#define SENSOR_IP IPV4(192,168,122,1)

#include <lib/littlefs.h>

typedef struct {
  uint16_t message_id; //2b
  //lk_time_t c_time;  
  uint8_t type; // 1b  options 0a:temp, 0b:humidity
  float value; // 4b	
  uint64_t c_time; // 8b coming time	
} sensor_data_t; //16b

lfs_t lfs_coap;

static int mount_lfs = 0;

static uint64_t coapcli_start_time;

static void coap_response_cb(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
	lfs_file_t file;
	LTRACEF("new coap request\n");
	
	uint8_t * buffer = data;
	LTRACEF("---------------\n");
	coap_packet_t cpacket;
	int rc=coap_parse(&cpacket,buffer,packetlen);
	if (rc == 0 && cpacket.payload.len > 0) {
	  ssize_t buflen = sizeof(sensor_data_t);
	  sensor_data_t datax;
	  
	  uint8_t bytes[4] = { cpacket.payload.p[0], cpacket.payload.p[1], cpacket.payload.p[2], cpacket.payload.p[3] };
	  memcpy (&datax.value, bytes, 4);
	  
	  //printf("response:%02x%02x:%f:%d\n",cpacket.hdr.id[0], cpacket.hdr.id[1], datax.value, buflen);
	
	  datax.message_id = cpacket.hdr.id[1] + (cpacket.hdr.id[0] << 8);
	  datax.type = 0x0a;
	  //sensor coming time
	  datax.c_time = current_time();
	  
	  // one line output
	  printf("response:%02x%02x:%f:%d\n",cpacket.hdr.id[0], cpacket.hdr.id[1], datax.value, buflen);
	  
	  
	  /*
	  lfs_file_open(&lfs_coap, &file, "sensor_data", LFS_O_RDWR | LFS_O_CREAT);
	  lfs_file_seek(&lfs_coap, &file, 0, LFS_SEEK_END);
	  lfs_file_write(&lfs_coap, &file, &datax.value, 4);
	  lfs_file_write(&lfs_coap, &file, "\n", 1);
	  // remember the storage is not updated until the file is closed successfully
	  lfs_file_close(&lfs_coap, &file);
	  */
	  
	}
}

static int coapcli_worker(void *argument) {
    int pull_count = *((int *) argument);
    
    // block device açma
    
    if (mount_lfs == 0) {
		LTRACEF("opening the block device\n");
		lfs_dev = bio_open(lfs_dev_name);
		if (!lfs_dev) { return -1; }
		int err = lfs_mount(&lfs_coap, &lfs_cfg);
		if (err) {
			lfs_format(&lfs_coap, &lfs_cfg);
			lfs_mount(&lfs_coap, &lfs_cfg);
		}
		mount_lfs = 1;
	}
    uint8_t packetbuf[32];
    int rc;
    // yanıt paket göndermek için
    udp_listen(UDP_PORT, (void *)&coap_response_cb, 0);
	udp_socket_t *socket;
	status_t st;
	LTRACEF("udp baglanti yapiliyor\n");
	if (udp_open(SENSOR_IP, UDP_PORT, UDP_PORT, &socket) != NO_ERROR) {
		printf("udp_open to failed\n");
		return -1;
	}
    LTRACEF("udp baglanti tm\n");
    // yanıt paketinin hazırlanması
    size_t rsplen = sizeof(packetbuf);
	coap_packet_t pkt;
	
	LTRACEF("coap paket hazirlaniyor0\n");
	// prepare coap request packet
	pkt.hdr.ver = 0x01;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 0;
    
    uint8_t mytoken = 0x01;
    pkt.tok.p = &mytoken ;
    pkt.tok.len = 1;
    pkt.hdr.tkl = pkt.tok.len ;
    LTRACEF("coap paket hazirlaniyor0.1\n");
    // başına 0 gelmiş oluyor.
    // sınıf olarak 0 requesti gösterir.
    pkt.hdr.code = COAP_METHOD_GET; // araştır get kodu olacak 0.xx
    //pkt->hdr.id[0] = msgid_hi;
    //pkt->hdr.id[1] = msgid_lo;
    pkt.numopts = 1;
    LTRACEF("coap paket hazirlaniyor1\n");
    // need token in response
    /*
    if (tok) {
        pkt->hdr.tkl = tok->len;
        pkt->tok = *tok;
    }
	*/
    // safe because 1 < MAXOPT
    pkt.opts[0].num = COAP_OPTION_URI_PATH;
    pkt.opts[0].buf.p = "temp";
    pkt.opts[0].buf.len = 4;
    pkt.payload.len = 0;
    LTRACEF("coap paket hazirlaniyor2\n");
	/*
	int l=pkt->payload.len;
	const uint8_t *_temp = pkt->payload.p;
    while(l--){printf("%x ",*pkt->payload.p++);};
    pkt->payload.p=_temp;
    _temp=NULL;
	*/
	
	for(int i=0; i < pull_count; i++) {
		pkt.hdr.id[0] = (uint8_t)rand(); //0x8c;
		pkt.hdr.id[1] = (uint8_t)rand(); //0x8c;
		if (0 != (rc = coap_build(packetbuf, &rsplen, &pkt))) {
			printf("respose package could not build:%d\n",rc);
		}
		else {
	      st = udp_send(packetbuf, rsplen, socket);
		  
		  /*
		  if (st < 0) {
			printf("send_data failed: %d\n", st);
		  } else {
			printf("data sent ok\n\n");
		  }
		  */
		  
		  thread_sleep(100);
	    }	
	}
	
	//st = udp_listen(UDP_PORT, (void *)&coap_response_cb, 0);
    return 0;
}

static int coapcli(int argc, const console_cmd_args *argv) {
	// argc kontrol ederek param geç todo!
	coapcli_start_time = current_time();
	int pull_count_r = 1;
	int pull_count_p = argv[1].i;
	if (pull_count_p > 0) {
		//thread_detach_and_resume(thread_create("Coapcli_Worker", &coapcli_worker, (void *) &pull_count_p, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
		thread_t *t1;
		int retcode = -1;
		for(int i=0;i<pull_count_p;i++){
		  //printf("-");
		  t1 = thread_create("Coapcli_Worker", &coapcli_worker, (void *) &pull_count_r, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
		  thread_resume(t1);
          thread_join(t1, &retcode, INFINITE_TIME);
		}
		printf("total time:%d\n",current_time()-coapcli_start_time);
	} else {
		for(int i=0;i<-1*pull_count_p;i++){
			thread_detach_and_resume(thread_create("Coapcli_Worker", &coapcli_worker, (void *) &pull_count_r, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
		}
	}
	//repeat 100 100 coapcli 1
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("coapcli", "coapcli tests", &coapcli)
STATIC_COMMAND_END(coapcli);

APP_START(coapcli)
APP_END

