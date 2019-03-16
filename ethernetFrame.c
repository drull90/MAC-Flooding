#include "ethernetFrame.h"

unsigned short checksum(unsigned short* buff, int _16bitword){
    unsigned long sum;
    for(sum = 0; _16bitword > 0; --_16bitword)
        sum += htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}