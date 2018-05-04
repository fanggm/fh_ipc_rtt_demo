import os
import sys
import re

if os.path.exists('appconfig.py'):
    import appconfig

# toolchains options
ARCH         = 'arm'
CPU          = 'armv6'
OUTPUT_NAME  = 'rtthread'
LD_NAME      = 'link'
COMPILE_NAME = 'self_compile_path'


#find the TextBase from .ld file.
Protect_file_path = LD_NAME + '.ld'
with open(Protect_file_path, 'r') as f:
    try:
        TextBase = re.findall(r'TextBase\s*=\s*(0x[A-Fa-f0-9]+)', f.read())[0]
    except IndexError:
        print "Error: %s does not define TextBase !" % Protect_file_path
        sys.exit(1)

CROSS_TOOL 	= 'gcc'
if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if CROSS_TOOL == 'gcc':
    PLATFORM 	= 'gcc'
    EXEC_PATH = os.environ['SCONS_EXEC_PATH_FH6X_RTT']
elif CROSS_TOOL == 'keil':
    PLATFORM 	= 'armcc'
    EXEC_PATH 	= 'C:/Keil'
elif CROSS_TOOL == 'iar':
    print '================ERROR============================'
    print 'Not support yet!'
    print '================================================='
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

print('EXEC_PATH is: %x',EXEC_PATH)

BUILD = 'release'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC = PREFIX + 'gcc'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    LINK = PREFIX + 'gcc'
    TARGET_EXT = 'axf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'
    DEVICE = ' -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=soft'
    CFLAGS = DEVICE + ' -mno-unaligned-access '
    CFLAGS += ' -Wall'
    CFLAGS += ' -Wno-error=sequence-point '
    CFLAGS += ' -fno-strict-aliasing '
    CFLAGS += ' -Wno-address '
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -D__ASSEMBLY__' + ' -DTEXT_BASE=' + TextBase
    LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map='+ OUTPUT_NAME +'.map,-cref,-u,_start -T' + LD_NAME +'.ld' + ' -Ttext ' + TextBase
    CPATH = ''
    LPATH = ''
    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2 '
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2'
    POST_ACTION = OBJCPY + ' -O binary $TARGET '+ OUTPUT_NAME +'.bin\n' + SIZE + ' $TARGET \n'
    # POST_ACTION+= 'cp ' + OUTPUT_NAME + '.bin /var/lib/tftpboot/rttm_8630.bin\n'
  #  POST_ACTION += OBJDUMP + ' -D  $TARGET > '+ OUTPUT_NAME +'.dis\n'
    if os.environ.has_key('SCONS_INSTALL_DIR_FH6X_RTT'):
        INSTALL_DIR = os.environ['SCONS_INSTALL_DIR_FH6X_RTT']
        if os.name == 'posix':
            POST_ACTION += 'cp ' + OUTPUT_NAME+ '.bin ' + INSTALL_DIR + '\n'
        else:
            POST_ACTION += 'cmd /c copy ' + OUTPUT_NAME + '.bin /Y ' + INSTALL_DIR +'\n'
elif PLATFORM == 'armcc':
    # toolchains
    CC = 'armcc'
    AS = 'armasm'
    AR = 'armar'
    LINK = 'armlink'
    TARGET_EXT = 'axf'
    DEVICE = ' --device DARMATS9'
    CFLAGS = DEVICE + ' --apcs=interwork --diag_suppress=870'
    AFLAGS = DEVICE
    LFLAGS = DEVICE + ' --strict --info sizes --info totals --info unused --info veneers --list rtthread-at91sam9260.map --ro-base 0x20000000 --entry Entry_Point --first Entry_Point'
    CFLAGS += ' -I"' + EXEC_PATH + '/ARM/RV31/INC"'
    LFLAGS += ' --libpath "' + EXEC_PATH + '/ARM/RV31/LIB"'
    EXEC_PATH += '/arm/bin40/'
    if BUILD == 'debug':
        CFLAGS += ' -g -O0'
        AFLAGS += ' -g'
    else:
        CFLAGS += ' -O2'
    POST_ACTION = 'fromelf --bin $TARGET --output rtthread.bin \nfromelf -z $TARGET'
