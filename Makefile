#
# Makefile
#

CIRCLEHOME = ../circle-master

OBJS	= main.o kernel.o \
		CPCCore/CPCCoreEmu/Asic.o\
		CPCCore/CPCCoreEmu/Bus.o\
		CPCCore/CPCCoreEmu/CRC.o\
		CPCCore/CPCCoreEmu/CRTC.o \
		CPCCore/CPCCoreEmu/CRTC_0.o \
		CPCCore/CPCCoreEmu/CRTC_1.o \
		CPCCore/CPCCoreEmu/CRTC_2.o \
		CPCCore/CPCCoreEmu/CRTC_3_4.o \
		CPCCore/CPCCoreEmu/DiskGen.o\
		CPCCore/CPCCoreEmu/DMA.o\
		CPCCore/CPCCoreEmu/DskTypeManager.o\
		CPCCore/CPCCoreEmu/FDC.o\
		CPCCore/CPCCoreEmu/IDisk.o\
		CPCCore/CPCCoreEmu/KeyboardHandler.o\
		CPCCore/CPCCoreEmu/Memoire.o\
		CPCCore/CPCCoreEmu/Monitor.o \
		CPCCore/CPCCoreEmu/Motherboard.o\
		CPCCore/CPCCoreEmu/MultifaceII.o\
		CPCCore/CPCCoreEmu/PPI.o \
		CPCCore/CPCCoreEmu/PlayCity.o \
		CPCCore/CPCCoreEmu/PrinterDefault.o\
		CPCCore/CPCCoreEmu/PSG.o \
		CPCCore/CPCCoreEmu/Sig.o \
		CPCCore/CPCCoreEmu/simple_math.o \
		CPCCore/CPCCoreEmu/simple_regex.o \
		CPCCore/CPCCoreEmu/simple_stdio.o \
		CPCCore/CPCCoreEmu/simple_string.o \
		CPCCore/CPCCoreEmu/Tape.o\
		CPCCore/CPCCoreEmu/VGA.o \
		CPCCore/CPCCoreEmu/Z80_Full.o\
		CPCCore/CPCCoreEmu/Z84C30.o\
		

EXTRACLEAN = $(OBJS)
		
CFLAGS = -DMINIMUM_DEPENDENCIES -DNO_MULTITHREAD -ICPCCore/zlib_pi -DNOFILTER -DNOZLIB -DNO_RAW_FORMAT
LIBS	= $(CIRCLEHOME)/lib/libcircle.a $(CIRCLEHOME)/lib/fs/fat/libfatfs.a $(CIRCLEHOME)/lib/fs/libfs.a


include Rules.mk

