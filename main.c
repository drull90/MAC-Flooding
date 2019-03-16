#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include "ethernetFrame.h"

int main(){

    int DESTMAC0 = 0x30;
    int DESTMAC1 = 0x9C;
    int DESTMAC2 = 0x23;
    int DESTMAC3 = 0x05;
    int DESTMAC4 = 0x89;
    int DESTMAC5 = 0x8C;

    int SRCMAC0 = 0x0A;
    int SRCMAC1 = 0x0A;
    int SRCMAC2 = 0x0A;
    int SRCMAC3 = 0x0A;
    int SRCMAC4 = 0x0A;
    int SRCMAC5 = 0x0A;

    int total_len = 0;
    unsigned char* sendbuff;

    struct ifreq ifreq_i;
    struct ifreq ifreq_ip;
    struct ethhdr* eth;
    struct iphdr* iph;

    //Creamos un rawSocket
    int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if(sock_raw == -1)
        printf("Error abriendo el socket");

    //Obtenemos el index de nuestra interfaz de red
    memset(&ifreq_i, 0, sizeof(ifreq_i));
    strncpy(ifreq_i.ifr_name, "enp1s0", IFNAMSIZ - 1);

    if((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0)
        printf("Error en index\n");

    printf("Index = %i\n", ifreq_i.ifr_ifindex);

    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    strncpy(ifreq_ip.ifr_name, "enp1s0", IFNAMSIZ - 1);
    if(ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0)
        printf("Error en SIOCGIFADDR");

    //Construimos la cabezera ethernet

    sendbuff = (unsigned char*) malloc(64);
    memset(sendbuff, 0, 64);

    eth = (struct ethhdr*)(sendbuff);

    eth->h_source[0] = SRCMAC0;
    eth->h_source[1] = SRCMAC1;
    eth->h_source[2] = SRCMAC2;
    eth->h_source[3] = SRCMAC3;
    eth->h_source[4] = SRCMAC4;
    eth->h_source[5] = SRCMAC5;

    eth->h_dest[0] = DESTMAC0;
    eth->h_dest[1] = DESTMAC1;
    eth->h_dest[2] = DESTMAC2;
    eth->h_dest[3] = DESTMAC3;
    eth->h_dest[4] = DESTMAC4;
    eth->h_dest[5] = DESTMAC5;

    eth->h_proto = htons(ETH_P_IP); //Siguiente header sera el de la ip

    total_len += sizeof(struct ethhdr);

    // Cabezera de ip
    iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;                   // ???
    iph->version = 4;               // ???
    iph->tos = 16;                  //Tipo de servicio ip ???
    //iph->id = htons(10201);         //Identificador
    iph->ttl = 64;                  //Vida del paquete
    //iph->protocol = 17;              //Protocolo ip
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in*)&(ifreq_ip.ifr_addr))->sin_addr))); //Ip source

    total_len += sizeof(struct iphdr);

    iph->tot_len = htons(total_len - sizeof(struct ethhdr));

    iph->check = 0; //mychecksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));

    //Enviando el frame

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = 2;
    sadr_ll.sll_halen = ETH_ALEN;
    sadr_ll.sll_addr[0] = DESTMAC0;
    sadr_ll.sll_addr[1] = DESTMAC1;
    sadr_ll.sll_addr[2] = DESTMAC2;
    sadr_ll.sll_addr[3] = DESTMAC3;
    sadr_ll.sll_addr[4] = DESTMAC4;
    sadr_ll.sll_addr[5] = DESTMAC5;

    //Lo enviamos
    int send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr*)&sadr_ll, sizeof(struct sockaddr_ll));
    if(send_len < 0)
        printf("Error en sendto : sendlen = %i\n", send_len);
    else
        printf("Envio exitoso");    

    return 0;
}