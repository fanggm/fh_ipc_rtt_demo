/*
 * File      : sample_vlcview.c
 * This file is part of SOCs BSP for RT-Thread distribution.
 *
 * Copyright (c) 2017 Shanghai Fullhan Microelectronics Co., Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "dsp/fh_jpeg_mpi.h"
#include "isp/isp_api.h"
#include "bufCtrl.h"
#include "sample_common_isp.h"
#include "sample_opts.h"
#include "sample_vlcview.h"

#if defined (CONFIG_CHIP_FH8630) || defined(CONFIG_CHIP_FH8830)
#include "fh_fd_mpi.h"
#endif



static FH_SINT32 g_exit            = 1;
static rt_thread_t g_thread_isp    = 0;
static rt_thread_t g_thread_stream = 0;

#ifdef FH_USING_RTSP
#include "rtsp.h"
struct rtsp_server_context *g_rtsp_server = NULL;
#else
#include "pes_pack.h"
#endif


//close ae
int ae_off_on(int k)
{
    if(k>1)
        {   
        printf("close error\n");
        return -1;
        }

    API_ISP_AEAlgEn(k);

    return 0;
}
//set ae intt,sensor_gain,isp_gain
void ae_adjust(int ae_intt, int a_gain, int d_gain)
{
    int ret ;
    ISP_AE_INFO ae_set;

    if( a_gain< 0x40)
        {   
        printf("set ae param erro\n");
        }

    API_ISP_GetAeInfo(&ae_set);
    printf("intt:%d a_gain:%d d_gain:%d \n",ae_set.u32Intt,ae_set.u32SensorGain,ae_set.u32IspGain);
    printf("intt:%d\n",ae_set.u32IspGainShift);
    ae_set.u32Intt = (ae_intt>>4)<<4;

    ae_set.u32SensorGain = a_gain;

    ae_set.u32IspGain = d_gain;
    ae_set.u32IspGainShift = 6;
    ret = API_ISP_SetAeInfo(&ae_set);
        if(ret)
           printf("set ae info failed \n");

}

//get ae info
void ae_info()
{
    ISP_AE_INFO ae_set;
    API_ISP_GetAeInfo(&ae_set);
    printf("intt:%d a_gain:%d d_gain:%d \n",ae_set.u32Intt,ae_set.u32SensorGain,ae_set.u32IspGain);

}

void sample_vlcview_exit()
{
    rt_thread_t exit_process;

    exit_process = rt_thread_find("vlc_get_stream");
    if (exit_process) rt_thread_delete(exit_process);

    API_ISP_Exit();
    exit_process = rt_thread_find("vlc_isp");
    if (exit_process) rt_thread_delete(exit_process);

#ifdef FH_USING_COOLVIEW
    rt_thread_delay(100);
    exit_process = rt_thread_find("coolview");
    // if (exit_process) rt_thread_delete(exit_process);
    // we'd better just suspend this thread.
    if (exit_process) rt_thread_suspend(exit_process);
#endif
    FH_SYS_Exit();
}

void sample_vlcview_get_stream_proc(void *arg)
{
    FH_SINT32 ret, i;
    FH_ENC_STREAM_ELEMENT stream;
#ifndef FH_USING_RTSP
    struct vlcview_enc_stream_element stream_element;
#endif
    FH_SINT32 *cancel = (FH_SINT32 *)arg;

    while (!*cancel)
    {
        do
        {
            ret = FH_VENC_GetStream(0, &stream);
            if (ret == RETURN_OK)
            {
                #ifdef FH_USING_RTSP
                for (i = 0; i < stream.nalu_cnt; i++)
                {
                    rtsp_push_data(g_rtsp_server, stream.nalu[i].start, stream.nalu[i].length,
                                   stream.time_stamp);
                }
                #else
                stream_element.frame_type = stream.frame_type == I_SLICE ? VLCVIEW_ENC_H264_I_FRAME : VLCVIEW_ENC_H264_P_FRAME;
                stream_element.frame_len  = stream.length;
                stream_element.time_stamp = stream.time_stamp;
                stream_element.nalu_count = stream.nalu_cnt;
                for (i = 0; i < stream.nalu_cnt; i++)
                {
                    stream_element.nalu[i].start = stream.nalu[i].start;
                    stream_element.nalu[i].len   = stream.nalu[i].length;
                }
                vlcview_pes_stream_pack(0, stream_element);
                #endif
                FH_VENC_ReleaseStream(0);
            }
        } while (ret == RETURN_OK);
        rt_thread_delay(1);
    }
}

extern void enc_write_proc(char*);

void force_iframe(void *param)
{
    FH_VENC_RequestIDR(0);
}

int vlcview(char *dsp_ip, rt_uint32_t port_no)
{
    FH_VPU_SIZE vi_pic;
    FH_VPU_CHN_CONFIG chn_attr;
#if defined(CONFIG_FH_SMART_ENC) && (defined(CONFIG_CHIP_FH8630) || defined(CONFIG_CHIP_FH8830))
    FH_SMART_CHR_CONFIG cfg_param;
#else
    FH_CHR_CONFIG cfg_param;
#endif
    FH_SINT32 ret;
    int port = port_no;

    if (!g_exit)
    {
        printf("vicview is running!\n");
        return 0;
    }

    g_exit = 0;

    bufferInit((unsigned char *)FH_SDK_MEM_START, FH_SDK_MEM_SIZE);   

    /* Enable texture processing */
    enc_write_proc("text_on_0");

    /******************************************
     step  1: init media platform
    ******************************************/
    ret = FH_SYS_Init();
    if (RETURN_OK != ret)
    {
        printf("Error: FH_SYS_Init failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  2: set vpss resolution
    ******************************************/
    vi_pic.vi_size.u32Width  = VIDEO_INPUT_WIDTH;
    vi_pic.vi_size.u32Height = VIDEO_INPUT_HEIGHT;
    ret                      = FH_VPSS_SetViAttr(&vi_pic);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VPSS_SetViAttr failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  3: start vpss
    ******************************************/
    ret = FH_VPSS_Enable(0);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VPSS_Enable failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  4: configure vpss channel 0
    ******************************************/
    chn_attr.vpu_chn_size.u32Width  = CH0_WIDTH;
    chn_attr.vpu_chn_size.u32Height = CH0_HEIGHT;
    ret                             = FH_VPSS_SetChnAttr(0, &chn_attr);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VPSS_SetChnAttr failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  5: open vpss channel 0
    ******************************************/
    ret = FH_VPSS_OpenChn(0);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VPSS_OpenChn failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  6: create venc channel 0
    ******************************************/
    cfg_param.chn_attr.size.u32Width          = CH0_WIDTH;
    cfg_param.chn_attr.size.u32Height         = CH0_HEIGHT;
    cfg_param.rc_config.bitrate               = CH0_BIT_RATE;
    cfg_param.rc_config.FrameRate.frame_count = CH0_FRAME_COUNT;
    cfg_param.rc_config.FrameRate.frame_time  = CH0_FRAME_TIME;
    cfg_param.chn_attr.profile                = FH_PRO_MAIN;
    cfg_param.chn_attr.i_frame_intterval      = 30;
    cfg_param.init_qp                         = 26;
    cfg_param.rc_config.ImaxQP                = 38;
    cfg_param.rc_config.IminQP                = 20;
    cfg_param.rc_config.PmaxQP                = 40;
    cfg_param.rc_config.PminQP                = 20;
    cfg_param.rc_config.RClevel               = FH_RC_LOW;
    cfg_param.rc_config.RCmode                = FH_RC_CBR;
    cfg_param.rc_config.max_delay             = 8;
#if defined(CONFIG_FH_SMART_ENC) && (defined (CONFIG_CHIP_FH8630) || defined(CONFIG_CHIP_FH8830))
    cfg_param.smart_attr.smart_en             = FH_TRUE;
    cfg_param.smart_attr.texture_en           = FH_TRUE;
    cfg_param.smart_attr.backgroudmodel_en    = FH_TRUE;
    cfg_param.smart_attr.mbconsist_en         = FH_FALSE;

    cfg_param.smart_attr.gop_th.GOP_TH_NUM           = 4;
    cfg_param.smart_attr.gop_th.TH_VAL[0]            = 8;
    cfg_param.smart_attr.gop_th.TH_VAL[1]            = 15;
    cfg_param.smart_attr.gop_th.TH_VAL[2]            = 25;
    cfg_param.smart_attr.gop_th.TH_VAL[3]            = 35;
    cfg_param.smart_attr.gop_th.MIN_GOP[0]            = 380;
    cfg_param.smart_attr.gop_th.MIN_GOP[1]            = 330;
    cfg_param.smart_attr.gop_th.MIN_GOP[2]            = 270;
    cfg_param.smart_attr.gop_th.MIN_GOP[3]            = 220;
    cfg_param.smart_attr.gop_th.MIN_GOP[4]            = 160;
    ret = FH_SMART_ENC_CreateChn(0, &cfg_param);
#else
    ret = FH_VENC_CreateChn(0, &cfg_param);
#endif
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VENC_CreateChn failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  7: start venc channel 0
    ******************************************/
    ret = FH_VENC_StartRecvPic(0);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_VENC_StartRecvPic failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  8: bind vpss channel 0 with venc channel 0
    ******************************************/
    ret = FH_SYS_BindVpu2Enc(0, 0);
    if (RETURN_OK != ret)
    {
        printf("Error: FH_SYS_BindVpu2Enc failed with %d\n", ret);
        goto err_exit;
    }

#if defined(CONFIG_FH_SMART_ENC) && (defined (CONFIG_CHIP_FH8630) || defined(CONFIG_CHIP_FH8830))
    {
        unsigned int w,h;
        w = (CH0_WIDTH + 15) / 16 * 16 / 8;
        h = (CH0_HEIGHT + 15) / 16 * 16 / 8;
        FH_BGM_InitMem(w,h);
        printf("!!!FH_BGM_InitMem ok\n");

        FH_SIZE picsize;
        picsize.u32Width = w;
        picsize.u32Height = h;
        FH_BGM_SetConfig(&picsize);
        printf("!!!FH_BGM_SetConfig ok\n");

        FH_BGM_Enable();
        printf("!!!FH_BGM_Enable ok\n");

        FH_SYS_BindVpu2Bgm();
        printf("bind ok!\n");

    }
#endif
#if defined(CONFIG_FH_SMART_ENC) && defined(CONFIG_CHIP_FH8830)
    {
        FH_VPSS_FDInit();
        FH_VPSS_FDEnable();
        FH_FD_Init();
        FH_FD_Enable();
        FH_SYS_BindVpu2FD();
        FH_VPSS_EnableAutoFaceGbox(0);
    }
#endif
    /******************************************
     step  9: init ISP, and then start ISP process thread
    ******************************************/
    if (sample_isp_init() != 0)
    {
        goto err_exit;
    }
    g_thread_isp = rt_thread_create("vlc_isp", sample_isp_proc, &g_exit,
                                    3 * 1024, RT_APP_THREAD_PRIORITY, 10);
    rt_thread_startup(g_thread_isp);
    
    
    /******************************************
     step  10: init stream pack
    ******************************************/
#ifdef FH_USING_RTSP
# if FH_USING_RTSP_RTP_TCP
    g_rtsp_server = rtsp_start_server(RTP_TRANSPORT_TCP, port);
#else
    g_rtsp_server = rtsp_start_server(RTP_TRANSPORT_UDP, port);
# endif
    rtsp_play_sethook(g_rtsp_server, force_iframe, NULL);
#else
    ret = vlcview_pes_init();
    if (0 != ret)
    {
        printf("Error: vlcview_pes_init failed with %d\n", ret);
        goto err_exit;
    }
    vlcview_pes_send_to_vlc(0, dsp_ip, port);
#endif

    /******************************************
     step  11: get stream, pack as PES stream and then send to vlc
    ******************************************/
    g_thread_stream =
        rt_thread_create("vlc_get_stream", sample_vlcview_get_stream_proc,
                         &g_exit, 3 * 1024, RT_APP_THREAD_PRIORITY, 10);
    if (g_thread_stream != RT_NULL)
    {
        rt_thread_startup(g_thread_stream);
    }

// rt_thread_sleep(10);

#ifdef FH_USING_COOLVIEW
#include "dbi/dbi_over_tcp.h"
#include "dbi/dbi_over_udp.h"

    rt_thread_t thread_dbg;
    struct dbi_tcp_config tcp_conf;
    tcp_conf.cancel = &g_exit;
    tcp_conf.port = 8888;

    thread_dbg = rt_thread_find( "coolview" );
    if( thread_dbg == RT_NULL )
    {
        thread_dbg = rt_thread_create("coolview", (void *)tcp_dbi_thread, &tcp_conf,
                4 * 1024, RT_APP_THREAD_PRIORITY + 10, 10);
        if (thread_dbg != RT_NULL)
        {
            rt_thread_startup(thread_dbg);
        }
    }
    else
    {
        rt_thread_resume( thread_dbg );
    }
#endif

    return 0;

err_exit:
    g_exit = 1;
    sample_vlcview_exit();
#ifdef FH_USING_RTSP
    rtsp_stop_server(g_rtsp_server);
#else
    vlcview_pes_uninit();
    deinit_stream_pack();
#endif

    return -1;
}

int vlcview_exit()
{
    if (!g_exit)
    {
        g_exit = 1;
        sample_vlcview_exit();
#ifdef FH_USING_RTSP
        rtsp_stop_server(g_rtsp_server);
#else
        vlcview_pes_uninit();
        deinit_stream_pack();
#endif
    }
    else
    {
        printf("vicview is not running!\n");
    }

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(vlcview, vlcview(dst_ip, port));
FINSH_FUNCTION_EXPORT(vlcview_exit, vlcview_exit());
FINSH_FUNCTION_EXPORT(ae_adjust, ae_adjust(intt,a_gain,d_gain));
FINSH_FUNCTION_EXPORT(ae_off_on, ae_off_on(bool));
FINSH_FUNCTION_EXPORT(ae_info, ae_info());

#endif
