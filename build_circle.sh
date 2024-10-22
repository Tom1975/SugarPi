echo "*** Build Circle ***"

#cp Rules.mk circle
#cp Rules.mk  circle-stdlib/libs/circle
cd circle-stdlib

# configure circle-std
#make clean
cp ../sysconfig.h  libs/circle/include/circle
make mrproper
./configure -r 4
make all

echo "*** End of Circle-std build ***"

cd libs/circle
cd addon/linux
make clean
make

cd ../SDCard
make clean
make

cd ../fatfs
make clean
make

cd ../vc4/sound
make clean
make

cd ../../vc4/interface/bcm_host
make clean
make

cd ../../../vc4/interface/vmcs_host
make clean
make

cd ../../../vc4/interface/vcos
make clean
make

cd ../../../vc4/vchiq
make clean
make

cd ../../../boot
make clean
make

#if [ "$AARCH" -eq 64 ];then
#   make armstub64
#else
    make armstub
#fi

echo "*** End of Circle build ***"
