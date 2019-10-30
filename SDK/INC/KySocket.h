#ifndef  KYSOCKET_H
#define  KYSOCKET_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "KyDefines.h"
#include "KyLog.h"

Int32_t Ky_udpSocketInit(string *ipstr, Uint16_t port);
Int32_t Ky_udpSocketInitNoAddr();

Int32_t Ky_tcpSocketInit(string *ipstr, Uint16_t port);
Int32_t Ky_tcpSocketInitNoAddr();

Int32_t Ky_udpSocketRecvfrom(Int32_t sockfd, void *buf, Int32_t buf_size, struct sockaddr_in *peer_addr);
Int32_t Ky_udpSocketSendto(Int32_t sockfd, void *buf, Int32_t buf_size, struct sockaddr_in *peer_addr);
void Ky_makeSockaddr(struct sockaddr_in *sock_addr, Uint32_t ip, Uint16_t port);
void Ky_resolveSockaddr(struct sockaddr_in *sock_addr, Int8_t * ip, Int32_t len, Uint16_t *port);

Int32_t Ky_sockaddrEqual(struct sockaddr_in *sock_addr1, struct sockaddr_in *sock_addr2);
Int8_t * Ky_getIpstr(struct sockaddr_in *sock_addr, Int8_t *ip);
Uint16_t Ky_getPort(struct sockaddr_in *sock_addr, Uint16_t *port);
void Ky_setSocketTimeout(Int32_t sockfd, Int32_t snd, Int32_t rcv);
void Ky_setUsefulSockOpt(Int32_t sockfd);
Int32_t Ky_createl2RawSocket(const char *if_name);

#endif

