#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/Users/john/ticcs/grace_3_10_00_82/packages;C:/Users/john/ticcs/msp430/msp430_driverlib_2_60_00_02/packages;C:/Users/john/ticcs/msp430/msp430_driverlib_2_60_00_02;C:/Users/john/ticcs/ccsv6/ccs_base;C:/Users/john/workspace_v6_1_3/solar-watering-system/msp430-adc/.config
override XDCROOT = C:/Users/john/ticcs/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/Users/john/ticcs/grace_3_10_00_82/packages;C:/Users/john/ticcs/msp430/msp430_driverlib_2_60_00_02/packages;C:/Users/john/ticcs/msp430/msp430_driverlib_2_60_00_02;C:/Users/john/ticcs/ccsv6/ccs_base;C:/Users/john/workspace_v6_1_3/solar-watering-system/msp430-adc/.config;C:/Users/john/ticcs/xdctools_3_32_00_06_core/packages;..
HOSTOS = Windows
endif
