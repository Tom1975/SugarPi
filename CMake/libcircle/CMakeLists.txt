project(libcircle)

include (../circle.cmake)

include_directories( 
                     ${CIRCLEHOME}/include 
                     ${CIRCLEHOME}/addon
                     ${CIRCLEHOME}/addon/vc4
                     ${CIRCLEHOME}/addon/vc4/interface/Khronos/include
                     ${CIRCLEHOME}/app/lib
                   )

set(Source_Files
   ${CIRCLEHOME}/lib/actled.cpp
   ${CIRCLEHOME}/lib/alloc.cpp
   ${CIRCLEHOME}/lib/bcmframebuffer.cpp 
   ${CIRCLEHOME}/lib/bcmmailbox.cpp
   ${CIRCLEHOME}/lib/bcmpropertytags.cpp 
   ${CIRCLEHOME}/lib/chargenerator.cpp 
   ${CIRCLEHOME}/lib/classallocator.cpp
   ${CIRCLEHOME}/lib/cputhrottle.cpp 
   ${CIRCLEHOME}/lib/debug.cpp 
   ${CIRCLEHOME}/lib/delayloop.S
   ${CIRCLEHOME}/lib/device.cpp 
   ${CIRCLEHOME}/lib/devicenameservice.cpp
   ${CIRCLEHOME}/lib/dmachannel.cpp 
   ${CIRCLEHOME}/lib/gpioclock.cpp 
   ${CIRCLEHOME}/lib/gpiomanager.cpp 
   ${CIRCLEHOME}/lib/gpiopin.cpp 
   ${CIRCLEHOME}/lib/gpiopinfiq.cpp
   ${CIRCLEHOME}/lib/i2cmaster.cpp 
   ${CIRCLEHOME}/lib/i2cslave.cpp 
   ${CIRCLEHOME}/lib/i2ssoundbasedevice.cpp 
   ${CIRCLEHOME}/lib/koptions.cpp
   ${CIRCLEHOME}/lib/logger.cpp 
   ${CIRCLEHOME}/lib/machineinfo.cpp 
   ${CIRCLEHOME}/lib/multicore.cpp 
   ${CIRCLEHOME}/lib/nulldevice.cpp 
   ${CIRCLEHOME}/lib/ptrarray.cpp 
   ${CIRCLEHOME}/lib/ptrlist.cpp
   ${CIRCLEHOME}/lib/pwmoutput.cpp 
   ${CIRCLEHOME}/lib/pwmsoundbasedevice.cpp 
   ${CIRCLEHOME}/lib/pwmsounddevice.cpp 
   ${CIRCLEHOME}/lib/qemu.cpp 
   ${CIRCLEHOME}/lib/screen.cpp 
   ${CIRCLEHOME}/lib/serial.cpp
   ${CIRCLEHOME}/lib/soundbasedevice.cpp 
   ${CIRCLEHOME}/lib/spimaster.cpp 
   ${CIRCLEHOME}/lib/spimasteraux.cpp 
   ${CIRCLEHOME}/lib/spimasterdma.cpp 
   ${CIRCLEHOME}/lib/spinlock.cpp
   ${CIRCLEHOME}/lib/string.cpp 
   ${CIRCLEHOME}/lib/sysinit.cpp 
   ${CIRCLEHOME}/lib/time.cpp 
   ${CIRCLEHOME}/lib/timer.cpp 
   ${CIRCLEHOME}/lib/tracer.cpp 
   ${CIRCLEHOME}/lib/usertimer.cpp 
   ${CIRCLEHOME}/lib/util.cpp
   ${CIRCLEHOME}/lib/util_fast.S
   ${CIRCLEHOME}/lib/virtualgpiopin.cpp 
   ${CIRCLEHOME}/lib/chainboot.cpp 
   ${CIRCLEHOME}/lib/macaddress.cpp 
   ${CIRCLEHOME}/lib/netdevice.cpp
   ${CIRCLEHOME}/lib/new.cpp 
   ${CIRCLEHOME}/lib/heapallocator.cpp 
   ${CIRCLEHOME}/lib/pageallocator.cpp 
   ${CIRCLEHOME}/lib/setjmp.S
   ${CIRCLEHOME}/lib/numberpool.cpp
   ${CIRCLEHOME}/lib/latencytester.cpp
   ${CIRCLEHOME}/lib/writebuffer.cpp
   )

set(OBJS32	
   ${CIRCLEHOME}/lib/cache-v7.S
   ${CIRCLEHOME}/lib/exceptionhandler.cpp
   ${CIRCLEHOME}/lib/exceptionstub.S
   ${CIRCLEHOME}/lib/memory.cpp
   ${CIRCLEHOME}/lib/pagetable.cpp
	${CIRCLEHOME}/lib/startup.S 
   ${CIRCLEHOME}/lib/synchronize.cpp
)

set(OBJS64
   ${CIRCLEHOME}/lib/exceptionhandler64.cpp 
   ${CIRCLEHOME}/lib/exceptionstub64.S 
   ${CIRCLEHOME}/lib/memory64.cpp 
   ${CIRCLEHOME}/lib/startup64.S
	${CIRCLEHOME}/lib/synchronize64.cpp 
   ${CIRCLEHOME}/lib/translationtable64.cpp
)

if ( ${AARCH} STREQUAL 32)
   set ( Source_Files ${Source_Files} ${OBJS32})
elseif (${AARCH} STREQUAL 64)
   set ( Source_Files ${Source_Files} ${OBJS64})
endif()

if ( ${RASPI} STREQUAL 4)
   set ( Source_Files ${Source_Files}  
      ${CIRCLEHOME}/lib/bcm54213.cpp
      ${CIRCLEHOME}/lib/bcmpciehostbridge.cpp
      ${CIRCLEHOME}/lib/bcmrandom200.cpp
      ${CIRCLEHOME}/lib/interruptgic.cpp
      ${CIRCLEHOME}/lib/dma4channel.cpp
      ${CIRCLEHOME}/lib/devicetreeblob.cpp
   )
else ()
   set ( Source_Files ${Source_Files} 
      ${CIRCLEHOME}/lib/bcmrandom.cpp
      ${CIRCLEHOME}/lib/ interrupt.cpp
   )
endif()

if ( NOT ${STDLIB_SUPPORT} STREQUAL 3)
set ( Source_Files ${Source_Files} ${CIRCLEHOME}/lib/purecall.cpp)
endif()


set(ALL_FILES ${Source_Files})
add_library(libcircle ${ALL_FILES})



