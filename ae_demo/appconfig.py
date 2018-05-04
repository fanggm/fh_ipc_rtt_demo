import os,sys

sdk_path = os.path.normpath(os.getcwd() + "/../../..")

os.environ["RTT_ROOT"] = sdk_path + "/bsp/rt-thread/kernel"
os.environ["SDK_ROOT"] = sdk_path + "/bsp/rt-thread"

if not os.getenv("RTT_EXEC_PATH"):
    if os.getenv("SCONS_EXEC_PATH_FH6X_RTT"):
        os.environ["RTT_EXEC_PATH"] = os.getenv("SCONS_EXEC_PATH_FH6X_RTT")
    else:
        os.environ["RTT_EXEC_PATH"] = "/opt/fullhan/toolchain/arm-2013.11/bin"
        if not os.path.exists("/opt/fullhan/toolchain/arm-2013.11/bin"):
            print "You must :"
            print "    (1) export RTT_EXEC_PATH=/path/to/toolchain"
            print "-or-"
            print "    (2) install toolchain first by running:"
            print
            print "        %s/docs_tools/software/pc/install_toolchain.sh" % sdk_path
            print
            sys.exit(2)

os.environ["SCONS_EXEC_PATH_FH6X_RTT"] = os.environ["RTT_EXEC_PATH"]

