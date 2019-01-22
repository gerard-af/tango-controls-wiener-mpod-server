cmake_minimum_required(VERSION 2.8.9)
project(WienerMpodLvHv)


set(SOURCES 
    main.cpp
    ClassFactory.cpp
    WienerMpodLvHvCtrlClass.cpp
    WienerMpodLvHvCtrlClass.h
    WienerMpodLvHvCtrl.cpp
    WienerMpodLvHvCtrl.h
    WienerMpodLvHvCtrlStateMachine.cpp
    wiener_mpod_lv_hv_types.h
    WienerMpodLvHvSnmp.h
    WienerMpodLvHvSnmp.cpp
    AfThreads.h
    AfThreads.cpp
)

include_directories(.)
## SHOULD INCLUDE TANGO AND NET-SNMP
link_directories(/usr/)
include_directories(/usr/include/)
include_directories(/usr/include/tango)
#include_directories(/home/gerard/Desktop/install/tango/include/tango)
#link_directories(/home/gerard/Desktop/install/tango/lib)

add_executable(WienerMpodLvHvCtrl ${SOURCES})
target_link_libraries(WienerMpodLvHvCtrl 
    /home/gerard/Desktop/install/tango/lib/libtango.so
    /home/gerard/Desktop/install/tango/lib/liblog4tango.so
    /usr/lib/x86_64-linux-gnu/libomniORB4.a 
    /usr/lib/x86_64-linux-gnu/libomnithread.a
    netsnmp
    pthread 
    dl
)
# debug
target_compile_options(WienerMpodLvHvCtrl PUBLIC ${TANGO_PKG_CFLAGS_OTHER} -Wall -Wextra -D_FORTIFY_SOURCE=2 -fpie)
# optimized
#target_compile_options(WienerMpodLvHvCtrl PUBLIC ${TANGO_PKG_CFLAGS_OTHER} -Wall -Wextra -D_FORTIFY_SOURCE=2 -O1 -fpie)

