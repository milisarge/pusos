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

#include <lib/datetime.h>

#define LOCAL_TRACE 1

#define NTP_TIMESTAMP_DELTA 2208988800

#define LI(packet)   (uint8_t) ((packet.li_vn_mode & 0xC0) >> 6) // (li   & 11 000 000) >> 6
#define VN(packet)   (uint8_t) ((packet.li_vn_mode & 0x38) >> 3) // (vn   & 00 111 000) >> 3
#define MODE(packet) (uint8_t) ((packet.li_vn_mode & 0x07) >> 0) // (mode & 00 000 111) >> 0


#define UDP_PORT 123
//#define NTP_IP IPV4(194,58,207,20)
#define NTP_IP IPV4(216,229,4,69)

// Structure that defines the 48 byte NTP packet protocol.

typedef struct
{

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.



static void ntp_response_cb(void *data, uint8_t packetlen,
                             uint32_t srcaddr, uint16_t srcport,
                             void *arg) {
	
	LTRACEF("new ntp response:%d\n",packetlen);

	ntp_packet * packet = (ntp_packet *)data;
	//ntp_packet packet;

	packet->txTm_s = ntohl( packet->txTm_s ); // Time-stamp seconds.
	packet->txTm_f = ntohl( packet->txTm_f ); // Time-stamp fraction of a second.

	// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
	// Subtract 70 years worth of seconds from the seconds since 1900.
	// This leaves the seconds since the UNIX epoch of 1970.
	// (1900)------------------(1970)**************************************(Time Packet Left the Server)

	lk_time_t txTm = ( lk_time_t ) ( packet->txTm_s - NTP_TIMESTAMP_DELTA );

	// Print the time we got from the server, accounting for local timezone and conversion from UTC time.
	DateTime _date;
	convertUnixTimeToDate(txTm, &_date);

	printf("NTP response: %d-%d-%d %d:%d:%d\n", 
	_date.year, _date.month, _date.day,
	_date.hours, _date.minutes, _date.seconds);  

}

static int ntptime(int argc, const console_cmd_args *argv) {
    // yanıt paket göndermek için
	status_t st;
    st = udp_listen(UDP_PORT, (void *)&ntp_response_cb, 0);
    LTRACEF("udp dinleme yapiliyor:%d\n",st);
	udp_socket_t *socket;
	LTRACEF("udp baglanti yapiliyor\n");
	st = udp_open(NTP_IP, UDP_PORT, UDP_PORT, &socket);
	LTRACEF("udp baglanti dönüt:%d\n",st);
	if (st != NO_ERROR) {
		printf("udp_open to failed\n");
		return -1;
	}
	
    LTRACEF("udp baglanti tm\n");
    // yanıt paketinin hazırlanması
   
   // Create and zero out the packet. All 48 bytes worth.

  ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  memset( &packet, 0, sizeof( ntp_packet ) );

  // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.

  *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

  // Create a UDP socket, convert the host-name to an IP address, set the port number,
  // connect to the server, send the packet, and then read in the return packet.
  st = udp_send(( char* )&packet,  sizeof( ntp_packet ) , socket);
  if (st < 0) {
	LTRACEF("send_data failed: %d\n", st);
  } else {
	LTRACEF("data sent ok\n\n");
  }
   
  return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("ntptime", "ntptime tests", &ntptime)
STATIC_COMMAND_END(ntptime);

APP_START(ntptime)
APP_END

