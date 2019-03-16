#ifndef ETHERNET_FRAME_H_
#define ETHERNET_FRAME_H_

unsigned short mychecksum(unsigned short* buff, int _16bitword);
void obtenerNombreInterfaz(char* interfaz);
void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw, char* if_name);
void construirCabezeraEthernet(struct ethhdr* eth);
void construirCabezeraIp(struct iphdr* iph, int total_len, struct ifreq*);
void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff);
void ponerMacDestino(struct macDest*);
void ponerMacOrigen(struct macSrc*);

struct macDest{
    int DESTMAC[6];
};

struct macSrc{
    int SRCMAC[6];
};

#endif