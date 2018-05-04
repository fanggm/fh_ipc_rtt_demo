#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include "sample_common_isp.h"
#include "multi_sensor.h"
#include "dsp/fh_vpu_mpi.h"
#include "isp_common.h"
#include "isp_api.h"
#include "isp_enum.h"
#include "sample_opts.h"
#include "gpio.h"

#define ERR_CNT 5

enum{
	NIGHT2DAY = 0,
	DAY2NIGHT = 1,
	IMPULSE,
	UNCHANGED,
};
FH_SINT32 g_isp_format = ISP_FORMAT;
FH_SINT32 g_isp_init_width = ISP_INIT_WIDTH;
FH_SINT32 g_isp_init_height = ISP_INIT_HEIGHT;

FH_UINT32 sample_isp_change_fps(void)
{
    FH_UINT32 fps = 0;

    //API_ISP_Pause();

    switch (g_isp_format)
    {
    case FORMAT_720P25:
        g_isp_format = FORMAT_720P30;
        fps          = 30;
        break;
    case FORMAT_720P30:
        g_isp_format = FORMAT_720P25;
        fps          = 25;
        break;
    case FORMAT_960P25:
        g_isp_format = FORMAT_960P30;
        fps          = 30;
        break;
    case FORMAT_960P30:
        g_isp_format = FORMAT_960P25;
        fps          = 25;
        break;
#if defined(FH8830) || defined(FH8630)
    case FORMAT_1080P25:
        g_isp_format = FORMAT_1080P30;
        fps = 30;
        break;
    case FORMAT_1080P30:
        g_isp_format = FORMAT_1080P25;
        fps = 25;
        break;
#endif
    }

    API_ISP_SetSensorFmt(g_isp_format);
    //API_ISP_Resume();

    return fps;
}

void isp_vpu_reconfig(void)
{
    FH_VPU_SIZE vpu_size;
    ISP_VI_ATTR_S isp_vi;

    FH_VPSS_Reset();
    API_ISP_Pause();
    API_ISP_Resume();

    FH_VPSS_GetViAttr(&vpu_size);
    API_ISP_GetViAttr(&isp_vi);
    if (vpu_size.vi_size.u32Width != isp_vi.u16PicWidth ||
        vpu_size.vi_size.u32Height != isp_vi.u16PicHeight)
    {
        vpu_size.vi_size.u32Width  = isp_vi.u16PicWidth;
        vpu_size.vi_size.u32Height = isp_vi.u16PicHeight;
        FH_VPSS_SetViAttr(&vpu_size);
    }

    API_ISP_KickStart();
}

#ifdef CONFIG_IRCUT_ON
unsigned int sensor_param_base;
unsigned int sensor_param_leng;

int sirStaPrev, simpulse_dly, sfrmDly, scam_mode;
static int sir_mode;
#endif
int sample_isp_init(void)
{
    FH_SINT32 ret;
    FH_UINT32 param_addr, param_size;
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;

    ret = API_ISP_MemInit(g_isp_init_width, g_isp_init_height);
    if (ret)
    {
        printf("Error: API_ISP_MemInit failed with %d\n", ret);
        return ret;
    }

    ret = API_ISP_GetBinAddr(&param_addr, &param_size);
    if (ret)
    {
        printf("Error: API_ISP_GetBinAddr failed with %d\n", ret);
        return ret;
    }

    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    ret = FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor);
    if (ret < 0)
    {
        rt_kprintf( "Sensor Init failed: %d\n", ret );
        return -1;
    }

    API_ISP_SensorRegCb(0, probed_sensor.sensor_handle);
    API_ISP_SensorInit();
    API_ISP_SetSensorFmt(g_isp_format);

    ret = API_ISP_Init();
    if (ret)
    {
        printf("Error: API_ISP_Init failed with %d\n", ret);
        return ret;
    }

#if defined(CONFIG_CHIP_FH8830) || defined(CONFIG_CHIP_FH8630)
	if (strcmp(probed_sensor.sensor_name, "imx291") == 0) {
		ret = API_ISP_SetCisClk(CIS_CLK_36M);
		if (ret) {
			printf("Error: API_ISP_SetCisClk(CIS_CLK_36M) failed with %d\n", ret);
			return ret;
		}
	}
#endif
    
#ifdef CONFIG_IRCUT_ON
    sensor_param_base = (unsigned int)probed_sensor.sensor_param;
    sensor_param_leng = (unsigned int)probed_sensor.param_len;
#endif

    API_ISP_LoadIspParam(probed_sensor.sensor_param);

#ifdef CONFIG_IRCUT_ON
    // ircut init.
    sir_mode = UNCHANGED;
    sirStaPrev = 1;
    simpulse_dly = 0;
    scam_mode = 1;
#endif

    return 0;
}

#define IR_IMPLUSE_WIDTH 10
#define IRDELAY 3
#define GAIN_DAY_TO_NIGHT 0x320
#define GAIN_NIGHT_TO_DAY 0x170
#define NIGHT 0
#define DAY 1

#ifdef CONFIG_IRCUT_ON
void ircut_ctrl( int onff, int enable, int prmload )
{
    int gpio = onff ? CONFIG_IRCUT_ON_GPIO : CONFIG_IRCUT_OFF_GPIO;

    gpio_request( gpio );
    gpio_direction_output( gpio, enable ? 1 : 0 );
    gpio_release( gpio );

    if( prmload )
    {
        API_ISP_LoadIspParam( (char *)(sensor_param_base + (onff ? sensor_param_leng : 0)) );
    }
}

void AutoIrcutCtrl()
{
	ISP_AE_INFO ae;
	static int irStaCurr = 1;
	API_ISP_GetAeInfo(&ae);

	irStaCurr = (ae.u32TotalGain > GAIN_DAY_TO_NIGHT)?NIGHT:irStaCurr;
	irStaCurr = (ae.u32TotalGain < GAIN_NIGHT_TO_DAY)?DAY:irStaCurr;

	if ((sir_mode == NIGHT2DAY) || (sir_mode == DAY2NIGHT))		// status change
	{
		if (irStaCurr != sirStaPrev) //status change
		{
			sfrmDly = 0;
			sir_mode = 1-irStaCurr;
		}
		if (sfrmDly > IRDELAY) // change mode
		{
			simpulse_dly = 0;		// impulse width count
			if (sir_mode == DAY2NIGHT)
			{
				ircut_ctrl( 1, 1, 1 );      // ircut_on, enable, load
			}
			else
			{
				ircut_ctrl( 0, 1, 1 );
			}
			scam_mode = 1 - sir_mode;				// 1:day  0:night
			sir_mode = IMPULSE;
		}
		sfrmDly++;
	}
	else if (sir_mode == IMPULSE)	// impulse output
	{
		simpulse_dly++;
		if (simpulse_dly > IR_IMPLUSE_WIDTH)
		{
			ircut_ctrl( 1, 0, 0 );
			ircut_ctrl( 0, 0, 0 );
			sir_mode = UNCHANGED;
		}
	}
	else if ((sir_mode == UNCHANGED))
	{
		if (scam_mode != irStaCurr)
		{
			sfrmDly = 0;
			sir_mode = 1-irStaCurr;
		}
	}

	sirStaPrev = irStaCurr;
}
#endif

void sample_isp_proc(void *arg)
{
    FH_SINT32 ret;
    FH_SINT32 err_cnt = 0;
    FH_SINT32 *cancel = (FH_SINT32 *)arg;

    while (!*cancel)
    {
        ret = API_ISP_Run();
        if (ret)
        {
            err_cnt++;
            ret = API_ISP_DetectPicSize();
            if (ret && err_cnt > ERR_CNT)
            {
                isp_vpu_reconfig();
                err_cnt = 0;
            }
        }
#ifdef CONFIG_IRCUT_ON
        AutoIrcutCtrl();
#endif
        rt_thread_delay(1);
    }
}
