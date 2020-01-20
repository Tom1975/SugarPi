./build_pi1.sh
./build_pi2.sh
./build_pi3.sh

echo "*** copy boot to sdcard ***"
cd circle/boot
make 
cd ../..

cp circle/boot/bootcode.bin sdcard
cp circle/boot/start.elf sdcard
cp circle/boot/start4.elf sdcard
cp circle/boot/fixup.dat sdcard
cp circle/boot/fixup4.dat sdcard
cp circle/boot/bcm2711-rpi-4-b.dtb sdcard
cp circle/boot/LICENCE.broadcom	sdcard
cp circle/boot/COPYING.linux sdcard
cp circle/boot/config.txt sdcard

echo "*** copy default CART sdcard ***"

if [ ! -d "sdcard/CART" ]; then
  mkdir sdcard/CART
fi
 
cp CART/* sdcard/CART