#ifndef ETHERNET_FRAME_H_
#define ETHERNET_FRAME_H_

unsigned short mychecksum(unsigned short* buff, int _16bitword);
void obtenerNombreInterfaz(char* interfaz);
void obtenerNumeroInterfaz(struct ifreq* ifreq_ip, int sock_raw);
void construirCabezeraEthernet(struct ethhdr* eth);
void construirCabezeraIp(struct iphdr* iph, int total_len);
void enviarFrame(struct sockaddr_ll* sadr_ll, int sock_raw, unsigned char* sendbuff);

#endif