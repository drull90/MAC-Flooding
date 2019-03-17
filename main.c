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

    int total_len = 0;
    char if_name[10];
    unsigned char* sendbuff;

    struct ifreq* ifreq_ip = malloc(sizeof(struct ifreq));
    struct ethhdr* eth;
    struct iphdr* iph;
    struct sockaddr_ll* sadr_ll;
    struct macDest* mdest = malloc(sizeof(struct macDest));
    struct macSrc* msrc = malloc(sizeof(struct macSrc));

    //Creamos un rawSocket
    int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

    //Verificamos que el socket se creo correctamente
    if(sock_raw == -1)
        printf("Error abriendo el socket");

    //Obtenemos el nombre de la interfaz a usar
    obtenerNombreInterfaz(if_name);

    //Obtenemos el numero de nuestra interfaz
    obtenerNumeroInterfaz(ifreq_ip, &sock_raw, if_name);

    //No importa la macdestino para hacer MAC Flooding
    ponerMacDestino(mdest);

    //Ponemos la mac origen
    ponerMacOrigen(msrc);

    //Construimos la cabezera ethernet
    sendbuff = (unsigned char*) malloc(64);
    memset(sendbuff, 0, 64);

    eth = (struct ethhdr*)(sendbuff);

    construirCabezeraEthernet(eth, msrc, mdest);

    total_len += sizeof(struct ethhdr);

    // Cabezera de ip
    iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));

    construirCabezeraIp(iph, &total_len, ifreq_ip);

    total_len += sizeof(struct iphdr);

    //Enviamos el frame
    //enviarFrame(sadr_ll, sock_raw, sendbuff, mdest);

    sadr_ll->sll_ifindex = 2;        //Numero de interfaz
    sadr_ll->sll_halen = ETH_ALEN;   //Tamaño de la direccion
    sadr_ll->sll_addr[0] = mdest->DESTMAC[0];
    sadr_ll->sll_addr[1] = mdest->DESTMAC[1];
    sadr_ll->sll_addr[2] = mdest->DESTMAC[2];
    sadr_ll->sll_addr[3] = mdest->DESTMAC[3];
    sadr_ll->sll_addr[4] = mdest->DESTMAC[4];
    sadr_ll->sll_addr[5] = mdest->DESTMAC[5];

    //Lo enviamos
    int send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr*)&sadr_ll, sizeof(struct sockaddr_ll));
    if(send_len < 0)
        printf("Error en sendto : sendlen = %i\n", send_len);
    else
        printf("Envio exitoso");   

    return 0;
}