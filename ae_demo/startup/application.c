/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author		Notes
 * 2011-01-13     weety		first version
 */

/**
 * @addtogroup FH8x30
 */
/*@{*/

#include <rtthread.h>
#include <rtdevice.h>
#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM FatFs filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#ifdef RT_USING_DFS_UFFS
/* dfs filesystem:UFFS filesystem init */
#include <dfs_uffs.h>
#endif
#endif

#if defined(RT_USING_DFS_DEVFS)
#include <devfs.h>
#endif

#ifdef RT_USING_SDIO
#include <drivers/mmcsd_core.h>
#endif

#ifdef RT_USING_LWIP
#include <netif/ethernetif.h>
//#include <arch/sys_arch_init.h>
//#include "macb.h"    TBD_WAIT_FILL ...
#endif

#ifdef RT_USING_LED
#include "led.h"
#endif

//#define RT_INIT_THREAD_STACK_SIZE (2*1024)
#define RT_INIT_THREAD_STACK_SIZE (10 * 1024)
#ifdef RT_USING_DFS_ROMFS
#include <dfs_romfs.h>
#endif

#ifdef RT_USING_W25QXX
#include <spi_flash_w25qxx.h>
#endif

#if defined(S_RT_USING_ADV_MODULE) && defined(S_RT_USING_SZDEMO)
#include "pubin_app.h"
#endif

#ifdef RT_USING_FH_DMA
#include "fh_dma.h"
#endif

#include "fh_def.h"
#include "clock.h"


extern void rt_board_driver_init();
extern void fh_media_process_module_init();
extern void fh_pae_module_init();
extern void fh_vpu_module_init();
extern void fh_jpeg_module_init();
extern void fh_vou_module_init();
extern void fh_isp_module_init();

#if defined(CONFIG_CHIP_FH8830) || defined(CONFIG_CHIP_FH8630)
extern rt_err_t fh_fd_module_init();
extern rt_err_t fh_bgm_module_init();
#endif

void rt_init_thread_entry(void *parameter)
{
    rt_board_driver_init();

#ifdef RT_USING_DSP
    {
        fh_media_process_module_init();
        fh_pae_module_init();
        fh_vpu_module_init();
        fh_jpeg_module_init();
        fh_vou_module_init();
#if defined(CONFIG_CHIP_FH8830) || defined(CONFIG_CHIP_FH8630)
        fh_fd_module_init();
        fh_bgm_module_init();
#endif
    }
#endif

#ifdef RT_USING_ISP
//#if defined(CONFIG_CHIP_FH8830) || defined(CONFIG_CHIP_FH8630)
//    reg_write(0xf0000028, 0x00131300);
//#else
    struct fh_clk *clk;
    clk = (struct fh_clk *)clk_get("cis_clk_out");
    if (!clk)
    {
        rt_kprintf("isp set sensor clk failed\n");
    }
    else
    {
        clk_set_rate(clk, 27000000);
    }
//#endif
    fh_isp_module_init();
#endif
}

int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init", rt_init_thread_entry, RT_NULL,
                                   RT_INIT_THREAD_STACK_SIZE, 8, 20);
#else
    init_thread = rt_thread_create("init", rt_init_thread_entry, RT_NULL,
                                   RT_INIT_THREAD_STACK_SIZE, 80, 20);
#endif

    if (init_thread != RT_NULL) rt_thread_startup(init_thread);

    return 0;
}

/* NFSv3 Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_LWIP) && defined(RT_USING_DFS_NFS)
#include <dfs_nfs.h>
void nfs_start(void)
{
    nfs_init();

    if (dfs_mount(RT_NULL, "/nfs", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
    {
        rt_kprintf("NFSv3 File System initialized!\n");
    }
    else
        rt_kprintf("NFSv3 File System initialzation failed!\n");
}
#ifdef RT_USING_FINSH
#include "finsh.h"
FINSH_FUNCTION_EXPORT(nfs_start, start net filesystem);
#endif
#endif

#ifdef RT_USING_FINSH
#include "finsh.h"
static unsigned int fh_hw_readreg( unsigned int addr )
{
    volatile unsigned int *paddr = (volatile unsigned int *)addr;

    return paddr[0];
}

long disp_memory(unsigned int addr, unsigned int length)
{
	unsigned int cnt = length >> 2;

	int i;
	// unsigned int *pmem = (unsigned int *)addr;

	rt_kprintf( "\r\nMemory info:[%#x,+%#x]\n", addr, length );
	for( i = 0; i < cnt; i ++ )
	{
		unsigned int v[4];
		int j;
		unsigned char *pb = (unsigned char *)v;
		v[0] = fh_hw_readreg(i * 16 + addr);
		v[1] = fh_hw_readreg(i * 16 + 4 + addr );
		v[2] = fh_hw_readreg(i * 16 + 8 + addr );
		v[3] = fh_hw_readreg(i * 16 + 12 + addr );
		rt_kprintf( "%#x: %08x %08x %08x %08x", addr + i * 16, \
					v[0], v[1], v[2], v[3] );
		rt_kprintf( " " );
		for( j = 0; j < 16; j ++ )
		{
			if( pb[j] > 31 && pb[j] < 127 )
			{
				rt_kprintf( "%c", pb[j] );
			}
			else
			{
				rt_kprintf( "." );
			}
		}
		rt_kprintf( "\n" );
	}
	if( length & 3 )
	{
		int j, cnt;
		int v[4];
		unsigned char *pb = (unsigned char *)v;

		cnt = length & 3;
		rt_kprintf( "%#x:", addr + i * 16 );
		for( j = 0; j < cnt; j ++ )
		{
			v[j] = fh_hw_readreg( i * 16 + addr + j * 4 );
			rt_kprintf( " %08x", v[j] );
		}
		for( j = 0; j < (4-cnt); j ++ )
			rt_kprintf( "         " );
		rt_kprintf( " " );
		for( j = 0; j < 4 * cnt; j ++ )
		{
			if( pb[j] > 31 && pb[j] < 127 )
				rt_kprintf( "%c", pb[j] );
			else
				rt_kprintf( "." );
		}
		rt_kprintf( "\n" );
	}

	/*
	 *
	 *
	 * fh_hw_readreg(i * 4 + 0), pmem[i * 4 + 1], pmem[i * 4 + 2], pmem[i * 4 + 3]
	 *
	 *
	 *
	 *
	 *
	 */

	rt_kprintf( "\r\n" );

	return 0;
}
FINSH_FUNCTION_EXPORT(disp_memory, display memory info)

static void fh_hw_writereg( unsigned int addr, unsigned int val )
{
    volatile unsigned int *paddr = (volatile unsigned int *)addr;

    paddr[0] = val;
}

long set_memory(unsigned int addr, unsigned int val)
{
    fh_hw_writereg( addr, val );
    return 0;
}
FINSH_FUNCTION_EXPORT(set_memory, set memory/register value)
#endif

