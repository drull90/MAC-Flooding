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

unsigned short checksum(unsigned short* buff, int _16bitword);

unsigned short checksum(unsigned short* buff, int _16bitword){
    unsigned long sum;
    for(sum = 0; _16bitword > 0; --_16bitword)
        sum += htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}


int main(){

    int DESTMAC0 = 0xB8;
    int DESTMAC1 = 0x27;
    int DESTMAC2 = 0xEB;
    int DESTMAC3 = 0xCF;
    int DESTMAC4 = 0x5A;
    int DESTMAC5 = 0x90;

    int SRCMAC0 = 0x0B;
    int SRCMAC1 = 0x0A;
    int SRCMAC2 = 0x0A;
    int SRCMAC3 = 0x0A;
    int SRCMAC4 = 0x0A;
    int SRCMAC5 = 0x0A;

    unsigned char* sendbuff;
    int total_len = 0;

    struct ifreq ifreq_i;
    struct ifreq ifreq_ip;

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

    struct ethhdr *eth = (struct ethhdr*)(sendbuff);

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
    struct iphdr* iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->id = htons(10201);
    iph->ttl = 64;
    iph->protocol = 17;
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in*)&(ifreq_ip.ifr_addr))->sin_addr)));
    iph->daddr = inet_addr("192.168.0.3");

    total_len += sizeof(struct iphdr);

    //Cabezera UDP
    struct udphdr* uh = (struct udphdr*)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));

    uh->source = htons(23451);
    uh->dest = htons(23452);
    uh->check = 0;

    total_len += sizeof(struct udphdr);

    //Añadimos payload
    
    sendbuff[total_len++] = 0xAA;
    sendbuff[total_len++] = 0xBB;
    sendbuff[total_len++] = 0xCC;
    sendbuff[total_len++] = 0xDD;
    sendbuff[total_len++] = 0xEE;

    uh->len = htons((total_len - sizeof(struct iphdr) - sizeof(struct ethhdr)));

    iph->tot_len = htons(total_len - sizeof(struct ethhdr));

    iph->check = checksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));

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