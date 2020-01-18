echo "*** Build target for Raspberry Pi 2 ***"

# Build Circle++
echo "*** Build Circle++ for Raspberry Pi 2 ***"
cd circle

echo RASPPI = 2 			 	> Config.mk
echo PREFIX = arm-none-eabi- 	>> Config.mk


./makeall clean
./makeall --nosample

cd boot
make 
cd ..

cd addon/linux
make clean
make

cd ../vc4/sound
make clean
make
cd ../vchiq
make clean
make

echo "*** Build Suagr Pi for Raspberry Pi 2 ***"
cd ../../../..

# check output dir
make clean
make 

# copy to target directory
if [ ! -d "pi2" ]; then
  mkdir pi2
fi
cd pi2
cp ../circle/boot/bootcode.bin 		.
cp ../circle/boot/start.elf			.
cp ../circle/boot/fixup.dat			.
cp ../circle/boot/LICENCE.broadcom 	.
cp ../circle/boot/COPYING.linux		.
cp ../kernel7.img						.
if [ ! -d "CART" ]; then
	mkdir CART
fi
cp ../CART/* ./CART

echo "*** End ***"
