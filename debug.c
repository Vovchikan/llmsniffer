#include "debug.h"

void hexdump (void *addr, int len) {
  int i;
  unsigned char buff[17];
  unsigned char sbuff[80];
  unsigned char *pbuff;
  size_t nc = 0;
  unsigned char *pc = (unsigned char*)addr;
  if (!addr) {fprintf(stderr, "(NULL)"); return; }
  /* process every byte in the data. */
  for (i=0;i<len;i++) {
    /* multiple of 16 means new line (with line offset). */
    if ((i%16) == 0) {
      pbuff = sbuff;
      /* just don't print  for the 0-th line. */
      if (i!=0) {
        nc += snprintf((char*) pbuff + nc, 80 - nc, "  %s\n", buff);
        fprintf (stderr, "%s", pbuff);
        nc = 0;
      }
      /* output the offset. */
      nc += snprintf((char*)pbuff + nc, 80 - nc, "  %04x ", i);
    }
    /* Now the hex code for the specific character. */
    nc += snprintf((char *)pbuff + nc, 80 - nc, " %02x", pc[i]);
    /* And store a printable ASCII character for later. */
    if (pc[i]>=0x20)  buff[i%16]=pc[i];
    else              buff[i%16]='.';
    buff[(i%16)+1]=0;
  }
  // Pad out last line if not exactly 16 characters.
  while ((i%16)!=0) { nc += snprintf((char*)pbuff + nc, 80 - nc, "   "); i++; }
  // And print the final ASCII bit.
  nc += snprintf((char*)pbuff + nc, 80 -nc,  "  %s\n", buff);
  fprintf(stderr, "%s", pbuff);
}

/* Display mac address */
void print_mac (unsigned char *mac) {
  printf("Mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" ,
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return;
}
