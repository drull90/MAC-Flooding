#include "ethernetFrame.h"
#include <arpa/inet.h>

unsigned short mychecksum(unsigned short* buff, int _16bitword){
    unsigned long sum;
    for(sum = 0; _16bitword > 0; --_16bitword)
        sum += htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum >> 16);
    return (unsigned short)(~sum);
} 