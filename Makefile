#
# Makefile
#

CIRCLEHOME = ../circle-master

OBJS	= main.o kernel.o \
		CPCCore/CPCCoreEmu/CRC.o CPCCore/CPCCoreEmu/FormatTypeDSK.o

#../../../03\ -\ SugarCore/FormatTypeDSK.o

LIBS	= $(CIRCLEHOME)/lib/libcircle.a

include Rules.mk

