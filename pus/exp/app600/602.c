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

#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"

#include "app600.h"

#define LOCAL_TRACE 0
// dönüş okumak dinlenecek port app a özel olmalı
#define UDP_PORT 5684
#define SENSOR_IP IPV4(192,168,122,1)

static void handle_humidity_resp(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
	LTRACEF("new coap request\n");
	
	uint8_t * buffer = data;
	LTRACEF("---------------\n");
	coap_packet_t cpacket;
	int rc=coap_parse(&cpacket,buffer,packetlen);
	if (rc == 0 && cpacket.payload.len > 0) {
	  //printf("data:%d:\n",cpacket.payload.len);
	  ////printf("data1:%x %x %x %x:\n",cpacket.payload.p[0],cpacket.payload.p[1],cpacket.payload.p[2],cpacket.payload.p[3]);
	  ////printf("data2:%x %x %x %x:\n",cpacket.payload.p[4],cpacket.payload.p[5],cpacket.payload.p[6],cpacket.payload.p[7]);
	  uint8_t bytes[4] = { cpacket.payload.p[0], cpacket.payload.p[1], cpacket.payload.p[2], cpacket.payload.p[3] };
	  int value;
	  memcpy (&value, bytes, 4);
	  
	  uint16_t message_id = cpacket.hdr.id[1] + (cpacket.hdr.id[0] << 8);
	  uint8_t type = 0x0a;
	  //sensor coming time
	  uint64_t c_time = rtc_current_time();
	  //printf("%d-%d-%d\n",message_id, type, c_time);
	  
	  mutex_acquire(&mu_lock_600);
	  char cmd[16];
	  sprintf(cmd, "humidity[%d]=%d;", message_id, value);
	  mu_eval(cmd, strlen(cmd), scope_600, 0);
	  mutex_release(&mu_lock_600);
	  
	}
}

static int app602(int argc, const console_cmd_args *argv) {
	//scope_600= mu_tbl_extend(0, MU_BUILTINS);
	if (mu_isnil(scope_600)){
		scope_600= mu_tbl_extend(0, MU_BUILTINS);
	}
	mutex_init(&mu_lock_600);
	
	mutex_acquire(&mu_lock_600);
    char *cmd = "humidity=[];";
    mu_eval(cmd, strlen(cmd), scope_600, 0);
	mutex_release(&mu_lock_600);
	
    uint8_t packetbuf[32];
    int rc;
    // yanıt paket göndermek için
    udp_listen(UDP_PORT, (void *)&handle_humidity_resp, 0);
	udp_socket_t *socket;
	status_t st;
	LTRACEF("udp baglanti yapiliyor\n");
	if (udp_open(SENSOR_IP, UDP_PORT, 5683, &socket) != NO_ERROR) {
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
    pkt.opts[0].buf.p = "humidity";
    pkt.opts[0].buf.len = 8;
    pkt.payload.len = 0;
    LTRACEF("coap paket hazirlaniyor2\n");
	
	for(int i=0;i<argv[1].i;i++) {
		pkt.hdr.id[0] = (uint8_t)rand(); //0x8c;
		pkt.hdr.id[1] = (uint8_t)rand(); //0x8c;
		if (0 != (rc = coap_build(packetbuf, &rsplen, &pkt))) {
			printf("respose package could not build:%d\n",rc);
		}
		else {
	      st = udp_send(packetbuf, rsplen, socket);
		  if (st < 0) {
			printf("send_data failed: %d\n", st);
		  } else {
			printf("data sent ok\n");
		  }
		  //thread_sleep(500);
	    }	
	}

    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app602", "app602 tests", &app602)
STATIC_COMMAND_END(app602);

APP_START(app602)
APP_END

