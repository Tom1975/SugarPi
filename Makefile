#
# Makefile
#

CIRCLEHOME = ../circle-master

OBJS	= main.o\
      kernel.o \
      DisplayPi.o\
      KeyboardPi.o\
		$(CIRCLEHOME)/addon/SDCard/emmc.o\
      $(CIRCLEHOME)/addon/fatfs/diskio.o\
      $(CIRCLEHOME)/addon/fatfs/ff.o\
      $(CIRCLEHOME)/addon/fatfs/ffunicode.o\
      $(CIRCLEHOME)/addon/fatfs/syscall.o\
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
		CPCCore/CPCCoreEmu/FormatTypeIPF.o\
		CPCCore/CPCCoreEmu/IDisk.o\
		CPCCore/CPCCoreEmu/KeyboardHandler.o\
		CPCCore/CPCCoreEmu/MediaManager.o\
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
		CPCCore/CPCCoreEmu/SoundMixer.o\
		CPCCore/CPCCoreEmu/Tape.o\
		CPCCore/CPCCoreEmu/VGA.o \
		CPCCore/CPCCoreEmu/YMZ294.o\
		CPCCore/CPCCoreEmu/Z80_Full.o\
		CPCCore/CPCCoreEmu/Z84C30.o\
		

EXTRACLEAN = $(OBJS)
		
OPTIMIZE = -O3
CFLAGS = -DMINIMUM_DEPENDENCIES -DNO_MULTITHREAD -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT  -I$(CIRCLEHOME)/addon 
LIBS	= $(CIRCLEHOME)/lib/libcircle.a $(CIRCLEHOME)/lib/fs/fat/libfatfs.a $(CIRCLEHOME)/lib/fs/libfs.a


include Rules.mk

