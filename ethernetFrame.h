#ifndef ETHERNET_FRAME_H_
#define ETHERNET_FRAME_H_

struct macDest{
    int DESTMAC[6];
};

struct macSrc{
    int SRCMAC[6];
};

unsigned short mychecksum(unsigned short*, int);
void obtenerNombreInterfaz(char*);
void obtenerNumeroInterfaz(struct ifreq*, int, char*, int*);
void construirCabezeraEthernet(struct ethhdr*, struct macSrc*, struct macDest*, int*);
void construirCabezeraIp(struct iphdr*, int, struct ifreq*, int*);
void enviarFrame(struct sockaddr_ll*, int, unsigned char*, struct macDest*, int);
void ponerMacDestino(struct macDest*);
void ponerMacOrigen(struct macSrc*, int);
void cambiarMacOrigenEthernet(struct ethhdr*, struct macSrc*);
void inizializarMacOrigen(struct macSrc*, int);
void sumarMac(struct macSrc*)
int modoDeUso(int*);

#endif