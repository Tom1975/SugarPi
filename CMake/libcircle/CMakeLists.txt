
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(circle)

include (../circle.cmake)

enable_language(ASM)

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
   ${CIRCLEHOME}/lib/assert.cpp
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

set( fs_Sources
   ${CIRCLEHOME}/lib/fs/partition.cpp
   ${CIRCLEHOME}/lib/fs/partitionmanager.cpp
)

set ( fsfat_Sources
   ${CIRCLEHOME}/lib/fs/fat/fatfs.cpp
   ${CIRCLEHOME}/lib/fs/fat/fatcache.cpp
   ${CIRCLEHOME}/lib/fs/fat/fatinfo.cpp
   ${CIRCLEHOME}/lib/fs/fat/fat.cpp
   ${CIRCLEHOME}/lib/fs/fat/fatdir.cpp
)

set( Sched_Sources
   ${CIRCLEHOME}/lib/sched/scheduler.cpp
   ${CIRCLEHOME}/lib/sched/synchronizationevent.cpp
   ${CIRCLEHOME}/lib/sched/mutex.cpp
   ${CIRCLEHOME}/lib/sched/task.cpp
   ${CIRCLEHOME}/lib/sched/taskswitch.S
)

set( Input_Sources
   ${CIRCLEHOME}/lib/input/keyboardbehaviour.cpp
   ${CIRCLEHOME}/lib/input/keymap.cpp
   ${CIRCLEHOME}/lib/input/mousebehaviour.cpp
   ${CIRCLEHOME}/lib/input/mouse.cpp
   ${CIRCLEHOME}/lib/input/touchscreen.cpp
	${CIRCLEHOME}/lib/input/console.cpp
   ${CIRCLEHOME}/lib/input/keyboardbuffer.cpp
   ${CIRCLEHOME}/lib/input/linediscipline.cpp
)

set( Usb_Sources
   ${CIRCLEHOME}/lib/usb/xhcidevice.cpp
   ${CIRCLEHOME}/lib/usb/lan7800.cpp
   ${CIRCLEHOME}/lib/usb/smsc951x.cpp
   ${CIRCLEHOME}/lib/usb/usbbluetooth.cpp
   ${CIRCLEHOME}/lib/usb/usbcdcethernet.cpp
   ${CIRCLEHOME}/lib/usb/usbconfigparser.cpp
   ${CIRCLEHOME}/lib/usb/usbdevice.cpp
   ${CIRCLEHOME}/lib/usb/usbdevicefactory.cpp
   ${CIRCLEHOME}/lib/usb/usbendpoint.cpp
   ${CIRCLEHOME}/lib/usb/usbfunction.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepad.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadps3.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadps4.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadstandard.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadswitchpro.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadxbox360.cpp
   ${CIRCLEHOME}/lib/usb/usbgamepadxboxone.cpp
   ${CIRCLEHOME}/lib/usb/usbhiddevice.cpp
   ${CIRCLEHOME}/lib/usb/usbhostcontroller.cpp
   ${CIRCLEHOME}/lib/usb/usbkeyboard.cpp
   ${CIRCLEHOME}/lib/usb/usbmassdevice.cpp
   ${CIRCLEHOME}/lib/usb/usbmidi.cpp
   ${CIRCLEHOME}/lib/usb/usbmouse.cpp
   ${CIRCLEHOME}/lib/usb/usbprinter.cpp
   ${CIRCLEHOME}/lib/usb/usbrequest.cpp
   ${CIRCLEHOME}/lib/usb/usbstandardhub.cpp
   ${CIRCLEHOME}/lib/usb/usbstring.cpp
   ${CIRCLEHOME}/lib/usb/usbserial.cpp
   ${CIRCLEHOME}/lib/usb/usbserialch341.cpp
   ${CIRCLEHOME}/lib/usb/usbserialcp2102.cpp
   ${CIRCLEHOME}/lib/usb/usbserialpl2303.cpp
   ${CIRCLEHOME}/lib/usb/usbserialft231x.cpp
   ${CIRCLEHOME}/lib/usb/usbserialcdc.cpp
)

set( Usb_Sources123
   ${CIRCLEHOME}/lib/usb/dwhcidevice.cpp
   ${CIRCLEHOME}/lib/usb/dwhciframeschednper.cpp
   ${CIRCLEHOME}/lib/usb/dwhciframeschednsplit.cpp
   ${CIRCLEHOME}/lib/usb/dwhciframeschedper.cpp
   ${CIRCLEHOME}/lib/usb/dwhciregister.cpp
   ${CIRCLEHOME}/lib/usb/dwhcirootport.cpp
   ${CIRCLEHOME}/lib/usb/dwhcixactqueue.cpp
   ${CIRCLEHOME}/lib/usb/dwhcixferstagedata.cpp
)

set( Usb_Sources4
   ${CIRCLEHOME}/lib/usb/xhcicommandmanager.cpp
   ${CIRCLEHOME}/lib/usb/xhcidevice.cpp
   ${CIRCLEHOME}/lib/usb/xhciendpoint.cpp
   ${CIRCLEHOME}/lib/usb/xhcieventmanager.cpp
   ${CIRCLEHOME}/lib/usb/xhcimmiospace.cpp
   ${CIRCLEHOME}/lib/usb/xhciring.cpp
   ${CIRCLEHOME}/lib/usb/xhciroothub.cpp
   ${CIRCLEHOME}/lib/usb/xhcirootport.cpp
   ${CIRCLEHOME}/lib/usb/xhcisharedmemallocator.cpp
   ${CIRCLEHOME}/lib/usb/xhcislotmanager.cpp
   ${CIRCLEHOME}/lib/usb/xhciusbdevice.cpp

)

set ( Addon_fatfs_Source
   ${CIRCLEHOME}/addon/fatfs/ff.c
   ${CIRCLEHOME}/addon/fatfs/diskio.cpp
   ${CIRCLEHOME}/addon/fatfs/ffsystem.cpp
   ${CIRCLEHOME}/addon/fatfs/ffunicode.c
)

set ( Vc4_Sound_Source
   ${CIRCLEHOME}/addon/vc4/sound/vchiqsoundbasedevice.cpp
   ${CIRCLEHOME}/addon/vc4/sound/vchiqsounddevice.cpp
)

set ( Vc4_VchiqDevice_Source
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiqdevice.cpp
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_arm.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_2835_arm.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_core.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_kern_lib.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_connected.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_shim.c
   ${CIRCLEHOME}/addon/vc4/vchiq/vchiq_util.c
)

set( Addon_SDCard_Sources
   ${CIRCLEHOME}/addon/SDCard/emmc.cpp
)


set( Addon_Linux_Sources
   ${CIRCLEHOME}/addon/linux/linuxdevice.cpp
   ${CIRCLEHOME}/addon/linux/linuxemu.cpp
	${CIRCLEHOME}/addon/linux/bug.cpp
   ${CIRCLEHOME}/addon/linux/completion.cpp
   ${CIRCLEHOME}/addon/linux/delay.cpp
   ${CIRCLEHOME}/addon/linux/device.cpp
   ${CIRCLEHOME}/addon/linux/dma-mapping.cpp
   ${CIRCLEHOME}/addon/linux/interrupt.cpp
   ${CIRCLEHOME}/addon/linux/kthread.cpp
	${CIRCLEHOME}/addon/linux/mutex.cpp
   ${CIRCLEHOME}/addon/linux/platform_device.cpp
   ${CIRCLEHOME}/addon/linux/printk.cpp
   ${CIRCLEHOME}/addon/linux/pthread.cpp
   ${CIRCLEHOME}/addon/linux/raspberrypi-firmware.cpp
   ${CIRCLEHOME}/addon/linux/rwlock.cpp
	${CIRCLEHOME}/addon/linux/semaphore.cpp
   ${CIRCLEHOME}/addon/linux/spinlock.cpp
   ${CIRCLEHOME}/addon/linux/sprintf.cpp
   ${CIRCLEHOME}/addon/linux/timer.cpp
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

set ( Source_Files ${Source_Files} ${Usb_Sources} ${Input_Sources} ${Sched_Sources} 
   ${fs_Sources} ${fsfat_Sources} ${Vc4_VchiqDevice_Source} ${Vc4_Sound_Source}
   ${Addon_Linux_Sources} ${Addon_SDCard_Sources} ${Addon_fatfs_Source}
)

if ( ${AARCH} STREQUAL 32)
   set ( Source_Files ${Source_Files} ${OBJS32} )
elseif (${AARCH} STREQUAL 64)
   set ( Source_Files ${Source_Files} ${OBJS64} )
endif()

if ( ${RASPI} STREQUAL 4)
   set ( Source_Files ${Source_Files}  ${Usb_Sources4}
      ${CIRCLEHOME}/lib/bcm54213.cpp
      ${CIRCLEHOME}/lib/bcmpciehostbridge.cpp
      ${CIRCLEHOME}/lib/bcmrandom200.cpp
      ${CIRCLEHOME}/lib/interruptgic.cpp
      ${CIRCLEHOME}/lib/dma4channel.cpp
      ${CIRCLEHOME}/lib/devicetreeblob.cpp
   )
else ()
   set ( Source_Files ${Source_Files} ${Usb_Sources123}
      ${CIRCLEHOME}/lib/bcmrandom.cpp
      ${CIRCLEHOME}/lib/ interrupt.cpp
   )
endif()

if ( NOT ${STDLIB_SUPPORT} STREQUAL 3)
set ( Source_Files ${Source_Files} ${CIRCLEHOME}/lib/purecall.cpp)
endif()


set(ALL_FILES ${Source_Files})
add_library(circle STATIC ${ALL_FILES})

