/* local */
#include "main.h"
#include "pkt_process.h" /* process_packet() */
#include "debug.h" /* hexdump () */

uint16_t udp_port = 55555;
char *ifname;

int main(int args, char* argv[]) {
  debug_argv(args, argv);

  int network_socket = Socket (AF_PACKET, SOCK_RAW,  htons(ETH_P_ALL));
  printf("OK: a raw socket is created.\n");;
  int process_socket = Socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  printf("OK: a udp socket is created.\n");;

  /* Создаём вспомогательные переменные, через которые получим
     мак адрес нужного сетевого интерфейса */
  struct ifreq ifr;
  bzero(&ifr, sizeof(ifr));
  ifr.ifr_addr.sa_family = AF_PACKET;
  strncpy(ifr.ifr_name , ifname , IFNAMSIZ-1);

  if (!ioctl(network_socket, SIOCGIFHWADDR, &ifr))
    print_mac((unsigned char *)ifr.ifr_hwaddr.sa_data);
  else {
    perror("icotl()");
    exit (3);
  }
  

  /* Получаем индекс интерфейса */
  struct ifreq ifr_ip;
  bzero(&ifr_ip, sizeof(ifr_ip));
  ifr_ip.ifr_addr.sa_family = AF_PACKET;
  strncpy(ifr_ip.ifr_name , ifname , IFNAMSIZ-1);
  if (ioctl(network_socket, SIOGIFINDEX, &ifr)) {
    perror("icotl() ifindex");
    exit (3);
  }

  /* Заполняем socket address */
  struct sockaddr_ll server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sll_family = AF_PACKET;
  server_address.sll_protocol = htons(ETH_P_ALL);
  server_address.sll_halen = ETH_ALEN;
server_address.sll_pkttype = PACKET_OUTGOING;
  memcpy (server_address.sll_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  server_address.sll_ifindex = ifr.ifr_ifindex;

  struct packet_mreq      mr;
  bzero(&mr, sizeof(mr));
  mr.mr_ifindex = ifr.ifr_ifindex;
  mr.mr_type    = PACKET_MR_PROMISC;
  setsockopt(network_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr));

  Bind(network_socket, (struct sockaddr*)&server_address, sizeof(server_address));

  /* Настраиваем udp сокет через который будем отправлять пакеты */
  struct sockaddr_in udp_sa;
  bzero(&udp_sa, sizeof(udp_sa));
  udp_sa.sin_family = AF_INET;
  udp_sa.sin_port = htons (udp_port);
  if (inet_pton(AF_INET, "127.0.0.1", &udp_sa.sin_addr) == -1) {
    perror ("udp_sa.sin_addr");
    exit (1);
  }


  /* Настраиваем структуры для приёма фреймов */
  uint8_t buffer[100];

  struct iovec iov[1];
  iov[0].iov_base = buffer;
  iov[0].iov_len = sizeof(buffer);

  struct msghdr message;
  message.msg_name = &server_address;
  message.msg_namelen = sizeof(server_address);
  message.msg_iov = iov;
  message.msg_iovlen = 1;
  message.msg_control = 0;
  message.msg_controllen = 0;

  printf("Receiving msg...\n");
  int count;
  for (;;) {
    count = recvmsg(network_socket, &message, 0);
    if (count == -1) {
      perror("recvmsg");
      exit(1);
    } else if (message.msg_flags & MSG_TRUNC) {
      /* frame too large for buffer: truncated */
    } else {
      /* Received msg */
      process_packet(process_socket, count, buffer, ifname, &udp_sa);
    }
  }

  close (network_socket);


  return 0;
}

void debug_argv (int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s <ifname> <udp-port>\n", argv[0]);
    exit(0);
  }

  int i;
  for (i = 0; i < argc; i++) {
    printf("Arg[%d] = %s\n", i, argv[i]);
  }

  ifname = argv[1];

  if ((udp_port = atoi(argv[2])) == 0) {
    printf("Error: can't convert udp port - %s\n", argv[2]);
    exit(1);
  }

  printf("udp port = %d\n", udp_port);

  return;
}
