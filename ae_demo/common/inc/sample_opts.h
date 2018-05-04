#ifndef SAMPLE_OPTS_H
#define SAMPLE_OPTS_H

#if defined(CONFIG_CHIP_FH8620) || defined(CONFIG_CHIP_FH8620G) || defined(CONFIG_CHIP_FH8810) || defined(CONFIG_CHIP_FH8622) || defined(CONFIG_CHIP_FH8812)
/* isp configuration */
#define ISP_FORMAT FORMAT_720P25
#define ISP_INIT_WIDTH 1280
#define ISP_INIT_HEIGHT 960

/* dsp configuration */
#define VIDEO_INPUT_WIDTH 1280
#define VIDEO_INPUT_HEIGHT 720

/* channel 0 configuration */
#define CH0_WIDTH 1280
#define CH0_HEIGHT 720
#define CH0_BIT_RATE (2048*1024)
#define CH0_FRAME_COUNT 25
#define CH0_FRAME_TIME 1

/* channel 1 configuration */
#define CH1_WIDTH 720
#define CH1_HEIGHT 576
#define CH1_BIT_RATE (512*1024)
#define CH1_FRAME_COUNT 25
#define CH1_FRAME_TIME 1

/* channel 2 configuration */
#define CH2_WIDTH 352
#define CH2_HEIGHT 288
#define CH2_BIT_RATE (256*1024)
#define CH2_FRAME_COUNT 25
#define CH2_FRAME_TIME 1

#define JPEG_INIT_WIDTH 1280
#define JPEG_INIT_HEIGHT 720

#elif defined(CONFIG_CHIP_FH8830) || defined(CONFIG_CHIP_FH8630)
/* isp configuration */
#define ISP_FORMAT FORMAT_1080P25
#define ISP_INIT_WIDTH 1920
#define ISP_INIT_HEIGHT 1088

/* dsp configuration */
#define VIDEO_INPUT_WIDTH 1920
#define VIDEO_INPUT_HEIGHT 1080

/* channel 0 configuration */
#define CH0_WIDTH 1920
#define CH0_HEIGHT 1080
#define CH0_BIT_RATE (4096*1024)
#define CH0_FRAME_COUNT 25
#define CH0_FRAME_TIME 1

/* channel 1 configuration */
#define CH1_WIDTH 720
#define CH1_HEIGHT 576
#define CH1_BIT_RATE (1024*1024)
#define CH1_FRAME_COUNT 25
#define CH1_FRAME_TIME 1

/* channel 2 configuration */
#define CH2_WIDTH 352
#define CH2_HEIGHT 288
#define CH2_BIT_RATE (512*1024)
#define CH2_FRAME_COUNT 25
#define CH2_FRAME_TIME 1

#define JPEG_INIT_WIDTH 1920
#define JPEG_INIT_HEIGHT 1088
#endif

#endif