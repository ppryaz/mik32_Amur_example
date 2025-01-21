#ifndef NETWORK_H
#define NETWORK_H


#define NET_MIN_DINAMIC_PORT 49152
#define NET_MAX_PORT 65535

#define NET_HANDLE_RESULT_OK       0
#define NET_HANDLE_RESULT_DROP     1
#define NET_HANDLE_RESULT_REJECT   2

#define MAC_ADDRESS_SIZE 6
#define NET_MAC {0x00, 0x08, 0xdc, 0x00, 0xab, 0xce}

#define NET_IP 192, 168, 1, 245

extern const unsigned char macaddr[];

#define  NETWORK

void NetInit();
void NetHandleNetwork();
void NetHandleIncomingPacket(unsigned short length);
unsigned char *NetGetBuffer();

#endif
