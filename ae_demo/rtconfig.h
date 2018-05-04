/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/*
 * SECTION: General Setup
 */
#define RT_NAME_MAX 16                      /* RT_NAME_MAX */
#define RT_ALIGN_SIZE 4                     /* RT_ALIGN_SIZE */
#define RT_THREAD_PRIORITY_MAX 256          /* PRIORITY_MAX */
#define RT_TICK_PER_SECOND 100              /* Tick per Second */


/*
 * SECTION: Debug & Trace
 */
// #define RT_DEBUG
// #define SCHEDULER_DEBUG                  /* Scheduler Debug */
// #define RT_THREAD_DEBUG                  /* Thread Debug */
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_INTERRUPT_INFO
#define RT_USING_HOOK                       /* Using Hook */
#define RT_USING_TIMER_SOFT                 /* Using Software Timer */
#define RT_TIMER_THREAD_PRIO 8


/*
 * SECTION: IPC
 */
#define RT_USING_SEMAPHORE                  /* Using Semaphore */
#define RT_USING_MUTEX                      /* Using Mutex */
#define RT_USING_EVENT                      /* Using Event */
#define RT_USING_MAILBOX                    /* Using MailBox */
#define RT_USING_MESSAGEQUEUE               /* Using Message Queue */


/*
 * SECTION: Memory Management
 */
#define RT_USING_MEMPOOL                    /* Using Memory Pool Management*/
#define RT_USING_HEAP                       /* Using Dynamic Heap Management */
#define RT_USING_MEMHEAP                    /* Using Mem Heap Management */
// #define RT_USING_SMALL_MEM               /* Using Small MM */
#define RT_USING_SLAB                       /* Using SLAB Allocator */
#define RT_USING_MM_TRACE


/*
 * SECTION: Runtime Library
 */
#define RT_USING_LIBC                       /* runtime libc library */
// #define RT_USING_NEWLIB                  /* runtime newlib library */
#define RT_USING_PTHREADS
#define RT_USING_MODULE                     /* Using Module System */
#define RT_USING_LIBDL


/*
 * SECTION: Device System
 */
#define RT_USING_DEVICE                     /* Using Device System */
#ifdef RT_USING_DEVICE
# define RT_USING_DEVICE_IPC
# define RT_USING_SERIAL
#endif


/*
 * SECTION: Console Options
 */
#define RT_USING_CONSOLE
#ifdef RT_USING_CONSOLE
# define RT_CONSOLE_DEVICE_NAME UART_NAME
# define RT_CONSOLEBUF_SIZE 2048            /* the buffer size of console */
#else
# define RT_CONSOLE_DEVICE_NAME ""
#endif


/*
 * SECTION: Finsh, a C-Express Shell
 */
#define RT_USING_FINSH                      /* Using FinSH as Shell*/
#ifdef RT_USING_FINSH
# define FINSH_USING_SYMTAB                 /* Using symbol table */
# define FINSH_USING_DESCRIPTION
# define FINSH_THREAD_STACK_SIZE (4096 * 8)
#endif


/*
 * SECTION: C++ Support
 */
// #define RT_USING_CPLUSPLUS               /* Using C++ support */


/*
 * SECTION: File System Support
 */
#define RT_USING_DFS                        /* using DFS support */
#ifdef RT_USING_DFS
# define DFS_DEBUG
# define DFS_USING_WORKDIR
# define DFS_FILESYSTEMS_MAX 4              /* the max number of mounted filesystem */
# define DFS_FD_MAX 16                      /* the max number of opened files */
// # define DFS_CACHE_MAX_NUM 4             /* the max number of cached sector */
// # define RT_USING_MODBUS                 /* Enable freemodbus protocol stack*/
// # define RT_USING_DFS_RAMFS
# define RT_USING_DFS_DEVFS
// # define RT_USING_DFS_YAFFS2             /* YAFFS2 File System */
// # define RT_USING_DFS_NFS                /* NFS File System */
# ifdef RT_USING_DFS_NFS
#  define RT_NFS_HOST_EXPORT "192.168.1.5:/"
# endif
# define RT_USING_DFS_ELMFAT                /* FAT32 File System */
# ifdef RT_USING_DFS_ELMFAT
#  define RT_DFS_ELM_USE_LFN 2              /* use long file name feature */
#  define RT_DFS_ELM_REENTRANT
#  define RT_DFS_ELM_CODE_PAGE 936          /* define OEM code page */
#  define RT_DFS_ELM_CODE_PAGE_FILE         /* Using OEM code page file */
#  define RT_DFS_ELM_MAX_LFN 128            /* the max number of file length */
# endif

#endif //of RT_USING_DFS

  
/*
 * SECTION: lwip, a lightweight TCP/IP protocol stack
 */
#define RT_USING_LWIP                       /* Using lightweight TCP/IP protocol stack */
#ifdef RT_USING_LWIP
// # define RT_LWIP_USING_RT_MEM
// # define RT_LWIP_REASSEMBLY_FRAG
# define RT_LWIP_DNS
# define RT_LWIP_ICMP
# define RT_LWIP_IGMP                       /* Enable IGMP protocol */
# define RT_LWIP_UDP                        /* Enable UDP protocol */
# define RT_LWIP_UDP_PCB_NUM 8
# define RT_LWIP_TCP                        /* Enable TCP protocol */
# define RT_LWIP_TCP_PCB_NUM 8              /* the number of simulatenously active TCP connections*/
# define RT_LWIP_TCP_SND_BUF 1024 * 21      /* TCP sender buffer space */
# define RT_LWIP_TCP_WND 1024 * 8           /* TCP receive window. */
// # define RT_LWIP_SNMP                    /* Enable SNMP protocol */
// # define RT_LWIP_DHCP                       /* Using DHCP */
# ifdef RT_LWIP_DHCP
#  define IP_SOF_BROADCAST 1
#  define IP_SOF_BROADCAST_RECV 1
#  define LWIP_USING_DHCPD                  /* marvell has private dhcpd */
# endif
# define RT_LWIP_IPADDR0 192                /* ip address of target */
# define RT_LWIP_IPADDR1 168
# define RT_LWIP_IPADDR2 1
# define RT_LWIP_IPADDR3 30
# define RT_LWIP_GWADDR0 192                /* gateway address of target */
# define RT_LWIP_GWADDR1 168
# define RT_LWIP_GWADDR2 1
# define RT_LWIP_GWADDR3 1
# define RT_LWIP_MSKADDR0 255               /* mask address of target */
# define RT_LWIP_MSKADDR1 255
# define RT_LWIP_MSKADDR2 255
# define RT_LWIP_MSKADDR3 0
# define RT_LWIP_PBUF_NUM 16                /* the number of blocks for pbuf */
# define RT_LWIP_TCP_SEG_NUM 80             /* the number of simultaneously queued TCP */
# define RT_LWIP_TCPTHREAD_PRIORITY 100     /* thread priority of tcpip thread */
# define RT_LWIP_TCPTHREAD_MBOX_SIZE 32     /* mail box size of tcpip thread to wait for */
# define RT_LWIP_TCPTHREAD_STACKSIZE 8192   /* thread stack size of tcpip thread */
# define RT_LWIP_ETHTHREAD_PRIORITY 126     /* thread priority of ethnetif thread */
# define RT_LWIP_ETHTHREAD_MBOX_SIZE 32     /* mail box size of ethnetif thread to wait for */
# define RT_LWIP_ETHTHREAD_STACKSIZE 1024   /* thread stack size of ethnetif thread */
# define LWIP_NETIF_STATUS_CALLBACK 1
// # define RT_LWIP_DNS
// # define RT_LWIP_DEBUG                   /* Trace LwIP protocol */
# define LWIP_SO_RCVBUF 1                   /* LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.*/
# define RECV_BUFSIZE_DEFAULT 8192          /* If LWIP_SO_RCVBUF is used, this is the default value for recv_bufsize */
# define SO_REUSE 1
# define RT_USING_COMPONENTS_NET
#endif // of RT_USING_LWIP


/*
 * SECTION: Board Type
 */

#define CONFIG_CHIP_FH8630
#define CONFIG_BOARD_APP

/*
 * SECTION: VBus
 */
#define RT_USING_VBUS
#define RT_VBUS_CONFIG_CLIENT


#define RT_USING_WIFI
#ifdef RT_USING_WIFI
#define WIFI_SDIO 0                        /* the SDIO used by wifi */
// #define WIFI_USING_AP6181
// //#define RT_USING_LWIP_INP6000
// #define RT_USING_WIFI_MARVEL
#define WIFI_USING_RTL8189FTV
// # ifdef RT_USING_WIFI_MARVEL
// #define RT_USING_WIFI_MARVEL_8782 0
// #define RT_USING_WIFI_MARVEL_8801 1
// // #   define RT_TEST_WIFI
// #  define RT_LWIP_PBUF_POOL_BUFSIZE (1536)
// # endif
#endif // of RT_USING_WIFI

#ifndef WIFI_USING_RTL8189FTV
#define RT_TIMER_THREAD_STACK_SIZE 512
#else
#define RT_TIMER_THREAD_STACK_SIZE 0x600
#endif


/*
 * SECTION: Application
 *
 * NOTE: choice one sensor only if not multi_sensor
 */
#define FH_USING_COOLVIEW
#define FH_USING_MULTI_SENSOR

#define RT_USING_JXF22
#define RT_USING_AR0230


#define RT_USING_LIBVLC
#define RT_USING_SAMPLE_OVERLAY
#define RT_USING_SAMPLE_VENC
#define RT_USING_SAMPLE_VLCVIEW
#define RT_USING_SAMPLE_MJPEG
#define RT_APP_THREAD_PRIORITY 130
#define FH_USING_ADVAPI_ISP
#define FH_USING_ADVAPI_OSD

//#define FH_USING_RTSP
#ifdef FH_USING_RTSP
# define FH_USING_RTSP_RTP_TCP 1
# define FH_USING_RTSP_RTP_UDP 0
#else
# define FH_USING_PES_PACK
#endif

#define RT_USING_MTD
#define RT_USING_MTD_NOR

// #define CONFIG_IRCUT_ON

//#define RT_USING_SOFTCORE
#ifdef RT_USING_SOFTCORE
#ifndef RT_USING_VBUS
#error "softcore depends on VBUS, you must define RT_USING_VBUS."
#endif
#endif
#define FH_DGB_DSP_PROC
#define FH_DGB_ISP_PROC

// #define RT_USING_ENC28J60
#ifdef RT_USING_ENC28J60
#define ENC28J60_INT (7)                    /* GPIO number for interrupt */
#define ENC28J60_SPI_DEV ("ssi0_1")         /* SPI device name */
#endif

#define RT_USING_DFS_JFFS2
#include "platform_def.h"
#define RT_USING_COMPONENTS_PROFILING
#endif
