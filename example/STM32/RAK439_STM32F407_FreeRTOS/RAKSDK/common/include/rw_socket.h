/**
  ******************************************************************************
  * @file    rw_socket.h
  * @author  RAK439 module Design Team
  * @version V1.0.3
  * @date    02-March-2015
  * @brief   RAK439 module bsd socket Header File.
  *
  *          This file contains:
  *           - basic socket operation apis.
  * 
  ******************************************************************************
**/

#ifndef _RW_SOCKET_H_
#define _RW_SOCKET_H_

#include <stdlib.h>
#include <stdint.h>

typedef uint32_t   socklen_t; 

#define AF_INET                 (2) 
#define PF_INET                 (AF_INET)
#define AF_INET6                (3)  
#define PF_INET6                (AF_INET6)

//
// Socket types
//
#define	SOCK_STREAM             (1)  /* TCP */
#define	SOCK_DGRAM              (2)  /* UDP */
#define	SOCK_RAW                (3)  /* IP  */

/* Generic/TCP socket options          */
#define SO_MAXMSG          0x1010    /* get TCP_MSS (max segment size) */
#define SO_LINGER          0x0080    /* linger on close if data present    */

#define IPPROTO_IP     0
#define IPPROTO_ICMP   1
#define IPPROTO_IGMP   2  /* added for IP multicasting changes */
#define IPPROTO_TCP    6
#define IPPROTO_UDP    17
#define IPPROTO_IPV6      41 /* IPv6 header */
#define IPPROTO_ROUTING   43 /* IPv6 Routing header */
#define IPPROTO_FRAGMENT  44 /* IPv6 fragmentation header */


struct linger {
     int l_onoff; /* 0 = off, nozero = on */
     int l_linger; /* linger time */
};

typedef  struct sockaddr_in {
	uint16_t sin_port ;		//Port number
	uint16_t sin_family ;	        //AF_INET
	uint32_t sin_addr ;		//IPv4 Address
}SOCKADDR_IN;

typedef  struct in6_addr {
   uint8_t   addr[16];             /* 128 bit IPv6 address */
} IN6_ADDR_T;

typedef struct sockaddr_in6
{
   uint16_t        sin6_family ;   // AF_INET6 
   uint16_t        sin6_port ;     // transport layer port #
   uint32_t        sin6_flowinfo ; // IPv6 flow information 
   IN6_ADDR_T      sin6_addr ;      // IPv6 address 
   uint32_t        sin6_scope_id ; // set of interfaces for a scope 
} SOCKADDR_IN6;


#ifdef htonl
#undef htonl
#endif /* htonl */
#ifdef ntohl
#undef ntohl
#endif /* ntohl */
#ifdef htons
#undef htons
#endif /* htons */
#ifdef ntohs
#undef ntohs
#endif /* ntohs */

#define htons(n) 				(((n & 0xff) << 8) | ((n & 0xff00) >> 8))
#define htonl(n) 				(((n & 0xff) << 24) |\
			    		     	((n & 0xff00) << 8) |\
			   			        ((n & 0xff0000UL) >> 8) |\
			    		    	((n & 0xff000000UL) >> 24))

#define ntohs(n) 				htons(n)
#define ntohl(n) 				htonl(n)


int socket(int domain, int type, int protocol);

int bind(int sockfd,const void *myaddr, socklen_t addrlen);

int connect(int sockfd, void *serv_addr, int addrlen);

int listen(int sockfd, int backlog);

int accept(int sockfd, void *addr, int *addrlen);

int send(int sockfd, const void *msg, int len, int flags);

int sendto(int sockfd, const void *data, size_t size, int flags, const void *to, socklen_t tolen);

int recv(int sockfd, void *buf, int len, unsigned int flags);

int recvfrom(int sockfd, void *mem, size_t len, int flags, void *from, socklen_t *fromlen);

int shutdown(int sockfd, int how);

int close(int sockfd);

int gethostbyname(const char *hostname ,uint8_t namelen ,uint32_t* out_ipaddr,uint16_t family);

int select(int sockfd, uint32_t timeout_ms);

int setsockopt(int sockfd, int level, int optname, const char* optval, int optlen);

int getsockopt(int sockfd, int level, int optname, void *optval, int optlen);


#endif

