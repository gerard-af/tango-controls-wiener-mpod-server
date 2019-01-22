cmake_minimum_required(VERSION 2.8.9)
project(WienerMpodHvModule)


set(SOURCES 
    main.cpp
    ClassFactory.cpp
    WienerMpodHvModuleClass.cpp
    WienerMpodHvModuleClass.h
    WienerMpodHvModule.cpp
    WienerMpodHvModule.h
    WienerMpodHvModuleDynAttrUtils.cpp
    WienerMpodHvModuleStateMachine.cpp
    WienerMpodHvModuleDynAttrExtensions.h
)

include_directories(.)
## SHOULD INCLUDE TANGO AND NET-SNMP
link_directories(/usr/)
include_directories(/usr/include/)
include_directories(/usr/include/tango)

add_executable(WienerMpodHvModule ${SOURCES})
target_link_libraries(WienerMpodHvModule tango log4tango 
    #    /usr/lib/x86_64-linux-gnu/libomniORB4.a
    #    /usr/lib/x86_64-linux-gnu/libomnithread.a
    #    /usr/lib/x86_64-linux-gnu/libomniDynamic4.a
        omniORB4
        omnithread
        omniDynamic4
    netsnmp
    pthread 
    dl
    /usr/lib/x86_64-linux-gnu/libomniDynamic4.a
)
# debug
target_compile_options(WienerMpodHvModule PUBLIC ${TANGO_PKG_CFLAGS_OTHER} -Wall -Wextra -D_FORTIFY_SOURCE=2 -fpie)
# optimized
#target_compile_options(WienerMpodHvModule PUBLIC ${TANGO_PKG_CFLAGS_OTHER} -Wall -Wextra -D_FORTIFY_SOURCE=2 -O1 -fpie)

