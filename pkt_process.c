#include "pkt_process.h"
#include "debug.h"

bool is_ipv4 (int, uint8_t*);
bool is_target_protocol (int, uint8_t*);
int insert_ifname (int, uint8_t*, int*, char*, int);
int spec_frame (int, int, uint8_t*, char*, struct sockaddr_in*);

/* sending bytes - maxlen = sizeof(buffer) */
uint8_t buffer[255];
int add_to_buffer (int bufferlen, uint8_t* buf, int* totallen, void *value, int value_len) {
  enum status st = ST_OK;
  int new_totallen = *totallen+value_len;

  if (bufferlen < new_totallen) {
    st = ST_BAD_VALUE;
    fprintf(stderr, "Can't add value to buffer, status = %d\n", st); 
    goto out;
  }

  memcpy(buf+*totallen, value, value_len);
  *totallen = new_totallen;

out:
  return st;
}


/* API */

void process_packet (int udp_socket, int pktlen, uint8_t *pkt, char* ifn, struct sockaddr_in *udp_sa) {
  if (!is_ipv4 (pktlen, pkt)) {
    /* EtherType != 0x8000, not IPv4 */
    return;
  }

  if (!is_target_protocol (pktlen, pkt)) {
    /* Don't need this packet */
    return;
  }

  int buflen = sizeof(buffer);
  int totallen = 0;


  if ( insert_ifname (buflen, buffer, &totallen, ifn, strlen(ifn)) != ST_OK)
    return;

  if ( add_to_buffer (buflen, buffer, &totallen, pkt, pktlen) != ST_OK) {
    fprintf (stderr, "Can't add pkt to udp buffer, buflen = %d, pkt_len = %d, totallen = %d\n",
        buflen, pktlen, totallen);
    return;
  }

  spec_frame (udp_socket, totallen, buffer, ifn, udp_sa);
}


/* Internal functions */

bool is_ipv4 (int len, uint8_t *pkt) {
  if (len < ETH_HLEN) /* This packet is less than Ethernet header */
    return false;

  return pkt[12] == 0x08 && pkt[13] == 0x00;
}

bool is_target_protocol (int len, uint8_t *pkt) {
  if (len < 25) /* Can't check Protocol field in IPv4 header */
    return false;

  return pkt[23] == OSPF_PROTOCOL; 
}

int insert_ifname (int buflen, uint8_t *buf, int *totallen, char *ifn, int namelen) {
  enum status st = ST_OK;

  if (buflen < namelen || namelen > IF_NAMESIZE-1) {
    st = ST_BAD_VALUE;
    goto out;
  }

  uint8_t  namelen8_t = (uint8_t)namelen;
  if ( (st = add_to_buffer (buflen, buf, totallen, &namelen8_t, sizeof(uint8_t)) ) != ST_OK) {
    fprintf (stderr, "Can't add namelen8_t = %d to buffer", namelen8_t);
    goto out;
  }

  if ( (st = add_to_buffer (buflen, buf, totallen, ifn, namelen) ) != ST_OK) {
    fprintf (stderr, "Can't add ifname = %s to buffer", ifn);
    goto out;
  }
  
out:
  return st;
}

int spec_frame (int udp_socket, int buflen, uint8_t *buf, char *ifn, struct sockaddr_in *udp_sa) {
  enum status st = ST_OK;

  hexdump (buf, buflen);
  sendto (udp_socket, buf, buflen, 0, (struct sockaddr *)udp_sa, sizeof(struct sockaddr_in));
  return st;
}
