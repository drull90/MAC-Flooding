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
    char* if_name = "";
    unsigned char* sendbuff;

    struct ifreq* ifreq_ip;
    struct ethhdr* eth;
    struct iphdr* iph;
    struct sockaddr_ll* sadr_ll;

    //Creamos un rawSocket
    int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

    //Verificamos que el socket se creo correctamente
    if(sock_raw == -1)
        printf("Error abriendo el socket");

    //Obtenemos el nombre de la interfaz a usar
    obtenerNombreInterfaz(if_name);

    //Obtenemos el numero de nuestra interfaz
    obtenerNumeroInterfaz(ifreq_ip, sock_raw, if_name);

    //Construimos la cabezera ethernet
    sendbuff = (unsigned char*) malloc(64);
    memset(sendbuff, 0, 64);

    eth = (struct ethhdr*)(sendbuff);

    construirCabezeraEthernet(eth);

    total_len += sizeof(struct ethhdr);

    // Cabezera de ip
    iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));

    construirCabezeraIp(iph, total_len, ifreq_ip);

    total_len += sizeof(struct iphdr);

    //Enviamos el frame
    enviarFrame(sadr_ll, sock_raw, sendbuff);

    return 0;
}