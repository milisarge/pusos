/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lk/err.h>
#include <lk/trace.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <lk/list.h>
#include <lk/compiler.h>
#include <endian.h>
#include <stdbool.h>
#include <lib/minip.h>
#include <platform.h>
#include <lib/coap.h>
#include "coapsrv.h"

#define LOCAL_TRACE 1

#define UDP_PORT 5699

//#define COAP_DEFAULT_PORT 5683

uint8_t packetbuf[64];

static uint8_t scratch_raw[64];
static coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};


static const coap_endpoint_path_t path_hello = {1, {"hello"}};
static int handle_get_hello(coap_rw_buffer_t *scratch, 
                            const coap_packet_t *inpkt, coap_packet_t *outpkt, 
                            uint8_t id_hi, uint8_t id_lo)
{
  static const char hello[64] = "Hello from PusOS Coap Server!";
  //printf("hello resp gönder\n");
  return coap_make_response(scratch, outpkt, 
                          (const uint8_t *)&hello, strlen(hello), 
                          id_hi, id_lo, &inpkt->tok, 
                          COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

coap_endpoint_t endpoints[] =
{
  {COAP_METHOD_GET, handle_get_hello, &path_hello, "ct=0"},
};

// endpoint

static void coap_svc_callback(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
	
	printf("new coap request\n");

	uint8_t * buffer = data;
	//printf("com_data:%s\n",(char *)buffer);
    //int32_t packetlen = len;

    //packetlen = (packetlen >= COAP_BUF_MAX_SIZE ? COAP_BUF_MAX_SIZE : packetlen);
	// yeni küt çalışma
	printf("---------------\n");
	coap_packet_t cpacket;
	int rc=coap_parse(&cpacket,buffer,packetlen);
	//coap_dumpPacket(&cpacket);
	
	// yanıt paket göndermek için
	udp_socket_t *socket;
	status_t st;
	if (udp_open(srcaddr, UDP_PORT, srcport, &socket) != NO_ERROR) {
		printf("udp_open to %u.%u.%u.%u:%u failed\n", IPV4_SPLIT(srcaddr), srcport);
		return ;
	}

    // yanıt paketinin hazırlanması
    size_t rsplen = sizeof(packetbuf);
	coap_packet_t rsppkt;
	rc = coap_handle_req(&scratch_buf, &cpacket, &rsppkt, endpoints);
	//printf("handle_req_rc:%d\n",rc);
	
	if (0 != (rc = coap_build(packetbuf, &rsplen, &rsppkt))) {
		printf("respose package could not build:%d\n",rc);
	}
	else {
		st = udp_send(packetbuf, rsplen, socket);
		if (st < 0) {
			printf("send_data failed: %d\n", st);
		} else {
			printf("data sent ok\n\n");
		}
	}
}

int coapsrv_init(void *arg) {
    status_t st = udp_listen(UDP_PORT, (void *)&coap_svc_callback, 0);
    return st;
}

