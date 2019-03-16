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

unsigned short mychecksum(unsigned short* buff, int _16bitword){
    unsigned long sum;
    for(sum = 0; _16bitword > 0; --_16bitword)
        sum += htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void obtenerNombreInterfaz(char* interfaz){
    printf("Introduce el nombre de la interfaz a usar\n");
    fflush(stdin);
    scanf("%s", interfaz);
    if(strcmp(interfaz, "") == 0)
        strcpy(interfaz, "enp1s0");
}

void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw, char* if_name){

    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    strncpy(ifreq_ip->ifr_name, if_name, IFNAMSIZ - 1);

    if(ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0)
        printf("Error en SIOCGIFADDR");

    printf("Index del interfaz : %i\n", ifreq_ip->ifr_ifindex);

}

void construirCabezeraEthernet(struct ethhdr* eth){

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

}

void construirCabezeraIp(struct iphdr* iph, int total_len, struct ifreq* ifreq_ip){

    iph->ihl = 5;                                                                               // ???
    iph->version = 4;                                                                           // ???
    iph->tos = 16;                                                                              //Tipo de servicio ip ???
    iph->ttl = 64;                                                                              //Vida del paquete
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in*)&(ifreq_ip->ifr_addr))->sin_addr))); //Ip source
    iph->tot_len = htons(total_len - sizeof(struct ethhdr));                                    // ???
    iph->check = 0;                                                                             //mychecksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));
    //iph->protocol = 17;                                                                       //Protocolo ip
    //iph->id = htons(10201);                                                                   //Identificador

}

void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff){

    sadr_ll->sll_ifindex = 2;        //Numero de interfaz
    sadr_ll->sll_halen = ETH_ALEN;   //Tamaño de la direccion
    sadr_ll->sll_addr[0] = DESTMAC0;
    sadr_ll->sll_addr[1] = DESTMAC1;
    sadr_ll->sll_addr[2] = DESTMAC2;
    sadr_ll->sll_addr[3] = DESTMAC3;
    sadr_ll->sll_addr[4] = DESTMAC4;
    sadr_ll->sll_addr[5] = DESTMAC5;

    //Lo enviamos
    int send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr*)&sadr_ll, sizeof(struct sockaddr_ll));
    if(send_len < 0)
        printf("Error en sendto : sendlen = %i\n", send_len);
    else
        printf("Envio exitoso");    

}

void ponerMacDestino(struct macDest* mdest){

    mdest->DESTMAC[0] = 0xAA;
    mdest->DESTMAC[1] = 0xAA;
    mdest->DESTMAC[2] = 0xAA;
    mdest->DESTMAC[3] = 0xAA;
    mdest->DESTMAC[4] = 0xAA;
    mdest->DESTMAC[5] = 0xAA;

}

void ponerMacOrigen(struct macSrc* msrc){
    int i;
    int hex;
    for( i = 1; i <= 6; ++i){
        do{
            printf("Introduce el bloque %i/6 de la mac origen", i);
            fflush(stdin);
            scanf("%x",&hex);
        }while(hex < 0x00 || hex > 0xFF);
        src->SRCMAC[i] = hex;
    }

}