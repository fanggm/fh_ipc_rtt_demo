gc0308 is a VGA sensor with max resolution of 640x480
Following step are NEEDED to run the gc0308 demo

1. modify common/isp/sample_common_isp.c

   -g_isp_format = FORMAT_720P25;
   +g_isp_format = FORMAT_VGAP25;

2. modify vlcview/sample_vlcview.c

   -#define PIC_WIDTH (1280)
   -#define PIC_HEIGHT (720)
   +#define PIC_WIDTH (640)
   +#define PIC_HEIGHT (480)

3. enable RT_USING_GC0308 in rtconfig.h

some modification needed, when multi-sensor with different resolution are enable at same time