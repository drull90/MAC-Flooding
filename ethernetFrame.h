#ifndef ETHERNET_FRAME_H_
#define ETHERNET_FRAME_H_

unsigned short mychecksum(unsigned short* buff, int _16bitword);
void obtenerNombreInterfaz(char* interfaz);
void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw, char* if_name);
void construirCabezeraEthernet(struct ethhdr* eth);
void construirCabezeraIp(struct iphdr* iph, int total_len, struct ifreq*);
void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff);

int DESTMAC0 = 0x30;
int DESTMAC1 = 0x9C;
int DESTMAC2 = 0x23;
int DESTMAC3 = 0x05;
int DESTMAC4 = 0x89;
int DESTMAC5 = 0x8C;

int SRCMAC0 = 0x0A;
int SRCMAC1 = 0x0A;
int SRCMAC2 = 0x0A;
int SRCMAC3 = 0x0A;
int SRCMAC4 = 0x0A;
int SRCMAC5 = 0x0A;

#endif