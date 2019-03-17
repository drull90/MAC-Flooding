#ifndef ETHERNET_FRAME_H_
#define ETHERNET_FRAME_H_

struct macDest{
    int DESTMAC[6];
};

struct macSrc{
    int SRCMAC[6];
};

unsigned short mychecksum(unsigned short* buff, int _16bitword);
void obtenerNombreInterfaz(char* interfaz);
void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw, char* if_name, int* number);
void construirCabezeraEthernet(struct ethhdr* eth, struct macSrc*, struct macDest*);
void construirCabezeraIp(struct iphdr* iph, int total_len, struct ifreq*);
void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff, struct macDest*, int if_number);
void ponerMacDestino(struct macDest*);
void ponerMacOrigen(struct macSrc*);
void cambiarMacOrigenEthernet(struct ethhdr* eth, struct macSrc* msrc);

#endif