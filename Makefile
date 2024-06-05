#
# Makefile
#

CIRCLEHOME = ./circle

OBJS	= src/main.o\
		src/kernel.o\
		src/BasicFrame.o \
		src/BackFrame.o \
		src/SugarPiSetup.o\
		src/ConfigurationManager.o\
		src/DisplayPi.o\
		src/DisplayPiImp.o\
		src/emulation.o\
		src/KeyboardPi.o\
		src/log.o\
		src/MainMenuWindows.o\
		src/Morphings.o \
		src/SoundPi.o\
		src/ScreenMenu.o\
		src/Window.o\
		res/SugarboxLogo.o\
		res/coolspot.o\
		CPCCore/CPCCoreEmu/Asic.o\
		CPCCore/CPCCoreEmu/Bus.o\
		CPCCore/CPCCoreEmu/CAPSFile.o\
		CPCCore/CPCCoreEmu/ClockLine.o\
		CPCCore/CPCCoreEmu/CRC.o\
		CPCCore/CPCCoreEmu/CRTC.o \
		CPCCore/CPCCoreEmu/CRTC_0.o \
		CPCCore/CPCCoreEmu/CRTC_1.o \
		CPCCore/CPCCoreEmu/CRTC_2.o \
		CPCCore/CPCCoreEmu/CRTC_3_4.o \
		CPCCore/CPCCoreEmu/DiskBuilder.o\
		CPCCore/CPCCoreEmu/DiskContainer.o\
		CPCCore/CPCCoreEmu/DiskGen.o\
		CPCCore/CPCCoreEmu/DMA.o\
		CPCCore/CPCCoreEmu/DskTypeManager.o\
		CPCCore/CPCCoreEmu/FDC.o\
		CPCCore/CPCCoreEmu/FileAccess.o\
		CPCCore/CPCCoreEmu/FormatTypeCTRAW.o\
		CPCCore/CPCCoreEmu/FormatTypeDSK.o\
		CPCCore/CPCCoreEmu/FormatTypeEDSK.o\
		CPCCore/CPCCoreEmu/FormatTypeHFE.o\
		CPCCore/CPCCoreEmu/FormatTypeHFEv3.o\
		CPCCore/CPCCoreEmu/FormatTypeIPF.o\
		CPCCore/CPCCoreEmu/IDisk.o\
		CPCCore/CPCCoreEmu/KeyboardHandler.o\
		CPCCore/CPCCoreEmu/MediaManager.o\
		CPCCore/CPCCoreEmu/MachineSettings.o\
		CPCCore/CPCCoreEmu/Memoire.o\
		CPCCore/CPCCoreEmu/Monitor.o \
		CPCCore/CPCCoreEmu/Motherboard.o\
		CPCCore/CPCCoreEmu/MultifaceII.o\
		CPCCore/CPCCoreEmu/PPI.o \
		CPCCore/CPCCoreEmu/PlayCity.o \
		CPCCore/CPCCoreEmu/PrinterDefault.o\
		CPCCore/CPCCoreEmu/PSG.o \
		CPCCore/CPCCoreEmu/rand.o \
		CPCCore/CPCCoreEmu/Sig.o \
		CPCCore/CPCCoreEmu/simple_filesystem.o \
		CPCCore/CPCCoreEmu/simple_math.o \
		CPCCore/CPCCoreEmu/simple_regex.o \
		CPCCore/CPCCoreEmu/simple_stdio.o \
		CPCCore/CPCCoreEmu/simple_string.o \
		CPCCore/CPCCoreEmu/Snapshot.o \
		CPCCore/CPCCoreEmu/SoundMixer.o\
		CPCCore/CPCCoreEmu/Tape.o\
		CPCCore/CPCCoreEmu/VGA.o \
		CPCCore/CPCCoreEmu/YMZ294.o\
		CPCCore/CPCCoreEmu/Z80_Full.o\
		CPCCore/CPCCoreEmu/Z80_Opcodes_fetch.o\
      CPCCore/CPCCoreEmu/Z80_Opcodes_ior.o\
      CPCCore/CPCCoreEmu/Z80_Opcodes_iow.o\
      CPCCore/CPCCoreEmu/Z80_Opcodes_memr.o\
      CPCCore/CPCCoreEmu/Z80_Opcodes_memw.o\
      CPCCore/CPCCoreEmu/Z80_Opcodes_z80wait.o\
		CPCCore/CPCCoreEmu/Z84C30.o\
		

EXTRACLEAN = $(OBJS)
		
OPTIMIZE = -O3

LIBS	= $(CIRCLEHOME)/lib/libcircle.a \
         $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
         $(CIRCLEHOME)/lib/fs/libfs.a \
         $(CIRCLEHOME)/lib/usb/libusb.a \
         $(CIRCLEHOME)/lib/input/libinput.a \
		 $(CIRCLEHOME)/lib/sound/libsound.a \
         $(CIRCLEHOME)/lib/sched/libsched.a \
         $(CIRCLEHOME)/addon/linux/liblinuxemu.a \
		 $(CIRCLEHOME)/addon/SDCard/libsdcard.a\
		 $(CIRCLEHOME)/addon/fatfs/libfatfs.a\
         $(CIRCLEHOME)/addon/vc4/sound/libvchiqsound.a\
         $(CIRCLEHOME)/addon/vc4/vchiq/libvchiq.a \
		 $(CIRCLEHOME)/addon/vc4/interface/bcm_host/libbcm_host.a \
		 $(CIRCLEHOME)/addon/vc4/interface/vmcs_host/libvmcs_host.a \
		 $(CIRCLEHOME)/addon/vc4/interface/vcos/libvcos.a 

 
include circle/Rules.mk

CFLAGS	+= -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_CUSTOM_OPCODES -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I$(CIRCLEHOME)/addon -DLOG_MIXER -DLOGFDC -DARM_ALLOW_MULTI_CORE
CPPFLAGS += -DMINIMUM_DEPENDENCIES -DUSE_VCHIQ_SOUND -DNO_CUSTOM_OPCODES -DNO_MULTITHREAD -I. -Isrc -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT -I$(CIRCLEHOME)/addon -DLOG_MIXER -DLOGFDC -DARM_ALLOW_MULTI_CORE -std=c++1z

