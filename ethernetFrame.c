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

    printf("Introduce el nombre de la interfaz a usar : \"0\" usara enp1s0\nPuede introducir ifconfig para ver las interfaces disponibles");
    fflush(stdin);
    fgets(interfaz, 10, stdin);

    if(strcmp(interfaz, "0\n") == 0 || interfaz == NULL)
        strcpy(interfaz, "enp1s0");

    printf("Interfaz : %s\n", interfaz);

}

void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw, char* if_name, int* number){

    memset(ifreq_ip, 0, sizeof(ifreq_ip));    
    strncpy(ifreq_ip->ifr_name, if_name, IFNAMSIZ - 1);

    if(ioctl(sock_raw, SIOCGIFADDR, ifreq_ip) < 0)
        printf("Error en SIOCGIFADDR\n");

    *number = ifreq_ip->ifr_ifindex;
    printf("Index del interfaz : %i\n", ifreq_ip->ifr_ifindex);

}

void construirCabezeraEthernet(struct ethhdr* eth, struct macSrc* msrc, struct macDest* mdest, int* total_len){

    eth = (struct ethhdr*)(sendbuff);

    eth->h_source[0] = msrc->SRCMAC[0];
    eth->h_source[1] = msrc->SRCMAC[1];
    eth->h_source[2] = msrc->SRCMAC[2];
    eth->h_source[3] = msrc->SRCMAC[3];
    eth->h_source[4] = msrc->SRCMAC[4];
    eth->h_source[5] = msrc->SRCMAC[5];

    eth->h_dest[0] = mdest->DESTMAC[0];
    eth->h_dest[1] = mdest->DESTMAC[1];
    eth->h_dest[2] = mdest->DESTMAC[2];
    eth->h_dest[3] = mdest->DESTMAC[3];
    eth->h_dest[4] = mdest->DESTMAC[4];
    eth->h_dest[5] = mdest->DESTMAC[5];

    eth->h_proto = htons(ETH_P_IP); //Siguiente header sera el de la ip

    total_len += sizeof(struct ethhdr);

}

void construirCabezeraIp(struct iphdr* iph, int total_len, struct ifreq* ifreq_ip, int* total_len){

    iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));

    iph->ihl = 5;                                                                                   // ???
    iph->version = 4;                                                                               // ???
    iph->tos = 1;                                                                                   //Tipo de servicio ip ???
    iph->ttl = 64;                                                                                  //Vida del paquete
    iph->saddr = inet_addr(inet_ntoa((( (struct sockaddr_in*) &ifreq_ip->ifr_addr )->sin_addr)));   //Ip source
    iph->tot_len = htons(total_len - sizeof(struct ethhdr));                                        // ???
    iph->check = 0;                                                                                 //mychecksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));
    iph->protocol = 29;                                                                             //Protocolo ip (iso-tp4) list of ip protocol numbers
    //iph->id = htons(10201);                                                                       //Identificador

    total_len += sizeof(struct iphdr);

}

void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff, struct macDest* mdest, int if_number){

    sadr_ll->sll_ifindex = if_number;        //Numero de interfaz
    sadr_ll->sll_halen = ETH_ALEN;   //Tamaño de la direccion
    sadr_ll->sll_addr[0] = mdest->DESTMAC[0];
    sadr_ll->sll_addr[1] = mdest->DESTMAC[1];
    sadr_ll->sll_addr[2] = mdest->DESTMAC[2];
    sadr_ll->sll_addr[3] = mdest->DESTMAC[3];
    sadr_ll->sll_addr[4] = mdest->DESTMAC[4];
    sadr_ll->sll_addr[5] = mdest->DESTMAC[5];

    //Lo enviamos
    int send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr*) sadr_ll, sizeof(struct sockaddr_ll));
    if(send_len < 0)
        printf("Error en sendto : sendlen = %i\n", send_len);
    else
        printf("Envio exitoso\n");    

}

void ponerMacDestino(struct macDest* mdest){

    mdest->DESTMAC[0] = 0xAA;
    mdest->DESTMAC[1] = 0xAA;
    mdest->DESTMAC[2] = 0xAA;
    mdest->DESTMAC[3] = 0xAA;
    mdest->DESTMAC[4] = 0xAA;
    mdest->DESTMAC[5] = 0xAA;

}

void inizializarMacOrigen(struct macSrc* msrc, int modo){

    if(modo == 0){

        for( i = 1; i <= 6; ++i){
            do{
                printf("Introduce el bloque %i/6 de MAC Origen desde donde empezar el ataquen\n", i);
                fflush(stdin);
                scanf("%x",&hex);
            }while(hex < 0x00 || hex > 0xFF);
            msrc->SRCMAC[i-1] = hex;
        }

    }

}

void ponerMacOrigen(struct macSrc* msrc, int modo){

    int i;
    int hex;

    if(modo == 0){                                  //Modo automatico

        sumarMac(msrc);

    }
    else{                                           //Modo manual

        for( i = 1; i <= 6; ++i){
            do{
                printf("Introduce el bloque %i/6 de la mac origen\n", i);
                fflush(stdin);
                scanf("%x",&hex);
            }while(hex < 0x00 || hex > 0xFF);
            msrc->SRCMAC[i-1] = hex;
        }

    }

}

void sumarMac(struct macSrc* msrc){

    int i = 0;

    msrc->SRCMAC[0] += 1;               //Sumamos 1 a la mac anterior

    for( i = 0; i < 4; ++i){            //Revisamos que la mac no sobrepase 0xFF, i < 4 => i + 1 = 5
        if(msrc->SRCMAC[i] > 0xFF){
            msrc->SRCMAC[i] = 0;
            msrc->SRCMAC[i+1] += 1;     //Max i+1 = 5
        }
    }

}

void cambiarMacOrigenEthernet(struct ethhdr* eth, struct macSrc* msrc){

    ponerMacOrigen(msrc);

    eth->h_source[0] = msrc->SRCMAC[0];
    eth->h_source[1] = msrc->SRCMAC[1];
    eth->h_source[2] = msrc->SRCMAC[2];
    eth->h_source[3] = msrc->SRCMAC[3];
    eth->h_source[4] = msrc->SRCMAC[4];
    eth->h_source[5] = msrc->SRCMAC[5];
}

int modoDeUso(int* repeticiones){

    int op;
    do{
        printf("Introduce 0 para uso automatico, o 1 para uso manual");
        scanf("%i", &op);
    }while(op < 0 || op > 1);

    if(op == 0){                                                //Uso automatico
        do{
            printf("Introduce el numero de Frames a enviar\n");
            fflush(stdin);
            scanf("%i", repeticiones);
        }while(*repeticiones < 0);
        
    }                         


    return op;
}