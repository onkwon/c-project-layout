#ifndef LWIP_PORT_LWIPOPTS_H
#define LWIP_PORT_LWIPOPTS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wstrict-overflow"

/** Debug options */
#define LWIP_DEBUG			1

#define ETHARP_DEBUG			LWIP_DBG_ON
#define NETIF_DEBUG 			LWIP_DBG_ON
#define PBUF_DEBUG			LWIP_DBG_ON
#define API_LIB_DEBUG			LWIP_DBG_ON
#define API_MSG_DEBUG			LWIP_DBG_ON
#define SOCKETS_DEBUG			LWIP_DBG_ON
#define ICMP_DEBUG			LWIP_DBG_ON
#define INET_DEBUG			LWIP_DBG_ON
#define IP_DEBUG			LWIP_DBG_ON
#define IP_REASS_DEBUG			LWIP_DBG_ON
#define RAW_DEBUG			LWIP_DBG_ON
#define MEM_DEBUG			LWIP_DBG_ON
#define MEMP_DEBUG			LWIP_DBG_ON
#define SYS_DEBUG			LWIP_DBG_ON
#define TCP_DEBUG			LWIP_DBG_ON
#define TCP_INPUT_DEBUG			LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG		LWIP_DBG_ON
#define TCP_RTO_DEBUG			LWIP_DBG_ON
#define TCP_CWND_DEBUG			LWIP_DBG_ON
#define TCP_WND_DEBUG			LWIP_DBG_ON
#define TCP_FR_DEBUG			LWIP_DBG_ON
#define TCP_QLEN_DEBUG			LWIP_DBG_ON
#define TCP_RST_DEBUG			LWIP_DBG_ON
#define UDP_DEBUG			LWIP_DBG_ON
#define TCPIP_DEBUG			LWIP_DBG_ON
#define PPP_DEBUG			LWIP_DBG_ON
#define SLIP_DEBUG			LWIP_DBG_ON
#define DHCP_DEBUG			LWIP_DBG_ON

/** System options */
#define NO_SYS				0
#define SYS_LIGHTWEIGHT_PROT		0

#define LWIP_STATS			0
#define LWIP_PROVIDE_ERRNO		1

/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down) */
#define LWIP_NETIF_LINK_CALLBACK	1

/** Memory options */
#define MEM_ALIGNMENT			4
/* MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE			4096
/* MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high. */
#define MEMP_NUM_PBUF			100
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB		6
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB		10
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN		5
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG		20
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT		10

/** pbuf options */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE			20
/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE		500

/** ICMP options */
#define LWIP_ICMP			1

/** DHCP options */
#define LWIP_DHCP			0

/** IP options */
#define LWIP_IPV4			1

/** UDP options */
#define LWIP_UDP			1
#define UDP_TTL				255

/** TCP options */
#define LWIP_TCP			1
#define TCP_TTL				255
/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ			0
/* TCP Maximum segment size. */
#if 0
#define TCP_MSS				\
	(1500/*ethernet MTU*/ - 24/*IP header*/ - 20/*TCP header*/)
#endif
/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF			(2 * TCP_MSS)
/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN		\
	((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
/* TCP receive window. */
#define TCP_WND				(4 * TCP_MSS)

/** Checksum options */
#if defined(CHECKSUM_BY_HARDWARE)
#define CHECKSUM_GEN_IP			0
#define CHECKSUM_GEN_UDP		0
#define CHECKSUM_GEN_TCP		0
#define CHECKSUM_CHECK_IP		0
#define CHECKSUM_CHECK_UDP		0
#define CHECKSUM_CHECK_TCP		0
#define CHECKSUM_GEN_ICMP		0
#else /* !CHECKSUM_BY_HARDWARE */
#define CHECKSUM_GEN_IP			1
#define CHECKSUM_GEN_UDP		1
#define CHECKSUM_GEN_TCP		1
#define CHECKSUM_CHECK_IP		1
#define CHECKSUM_CHECK_UDP		1
#define CHECKSUM_CHECK_TCP		1
#define CHECKSUM_GEN_ICMP		1
#endif /* CHECKSUM_BY_HARDWARE */

#if 0
/* OS options */
#define TCPIP_THREAD_NAME		"TCP/IP"
#define TCPIP_THREAD_STACKSIZE		2048
#define TCPIP_MBOX_SIZE			5
#define DEFAULT_UDP_RECVMBOX_SIZE	1024
#define DEFAULT_TCP_RECVMBOX_SIZE	1024
#define DEFAULT_ACCEPTMBOX_SIZE		1024
#define DEFAULT_THREAD_STACKSIZE	2048
#define TCPIP_THREAD_PRIO		3
#endif
#define LWIP_COMPAT_MUTEX		0

#endif /* LWIP_PORT_LWIPOPTS_H */
