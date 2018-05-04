/*
 * File      : vbus_local_conf.h
 * This file is part of FH8620 BSP for RT-Thread distribution.
 *
 * Copyright (c) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved
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
 *  Visit http://www.fullhan.com to get contact with Fullhan.
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __VBUS_LOCAL_CONF_H__
#define __VBUS_LOCAL_CONF_H__

// #define RT_VBUS_USING_FLOW_CONTROL

// #define LOG_PERFORMANCE_DATA
#ifdef LOG_PERFORMANCE_DATA
extern void vecho_log( unsigned int );
#else
#define vecho_log(x)
#endif

/* We don't use the IRQ number to trigger IRQ in this BSP. */
#define RT_VBUS_GUEST_VIRQ          60
#define RT_VBUS_HOST_VIRQ           61


/*
 * VBUS priority queue MUST be less than 32!!!
 */

#if (RT_THREAD_PRIORITY_MAX == 32)

#define RT_VBUS_BASE_PRIO           (20)
#define RT_VBUS_SER_PRIO            (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_RFS_PRIO            (RT_VBUS_BASE_PRIO + 3)
#define RT_VBUS_VOUCTL_PRIO         (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_VECHO_PRIO          (RT_VBUS_BASE_PRIO + 10)
#define RT_VBUS_ACCTRL_PRIO         (RT_VBUS_BASE_PRIO + 0)
#define RT_INIT_THRD_PRIO           (RT_VBUS_BASE_PRIO + 0)
#define RT_VOU_APP_PRIO             (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUSIN_DAEMON_PRIO       (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUSOUT_DAEMON_PRIO      (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_WORKER_PRIO         (RT_VBUS_BASE_PRIO + 0)
#define DISPATCHER_PRIORITY         (RT_VBUS_BASE_PRIO + 0)

#else

#define RT_VBUS_BASE_PRIO           (20)
#define RT_VBUS_SER_PRIO            (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_RFS_PRIO            (RT_VBUS_BASE_PRIO + 3)
#define RT_VBUS_VOUCTL_PRIO         (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_VECHO_PRIO          (RT_VBUS_BASE_PRIO + 10)
#define RT_VBUS_ACCTRL_PRIO         (RT_VBUS_BASE_PRIO + 0)
#define RT_INIT_THRD_PRIO           (RT_VBUS_BASE_PRIO + 0)
#define RT_VOU_APP_PRIO             (RT_VBUS_BASE_PRIO + 0)
#define RT_VBUS_WORKER_PRIO         (RT_VBUS_BASE_PRIO - 8)
#define DISPATCHER_PRIORITY         (RT_VBUS_BASE_PRIO - 10)

#define RT_VBUSIN_DAEMON_PRIO       (RT_VBUS_BASE_PRIO - 12)
#define RT_VBUSOUT_DAEMON_PRIO      (RT_VBUSIN_DAEMON_PRIO + 1)

#endif

#if RT_VBUSIN_DAEMON_PRIO == RT_THREAD_PRIORITY_MAX
#error "RT_VBUSOUT_DAEMON_PRIO too low"
#endif

// #define RT_VBUS_USING_TESTS

#endif /* end of include guard: __VBUS_LOCAL_CONF_H__ */
