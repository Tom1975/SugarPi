#./build_pi3.sh
# No longer used, as dispmanx need 32 bits application
#./build_pi4.sh
./build_pi4_32bits.sh

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
cp circle/boot/bcm2711-rpi-400.dtb sdcard
cp circle/boot/bcm2711-rpi-cm4.dtb sdcard
cp circle/boot/bcm2712-rpi-5-b.dtb sdcard
cp circle/boot/bcm2710-rpi-zero-2-w.dtb sdcard
cp circle/boot/LICENCE.broadcom	sdcard
cp circle/boot/COPYING.linux sdcard
cp circle/boot/start_cd.elf	sdcard
cp circle/boot/start4cd.elf	sdcard
cp circle/boot/fixup_cd.dat	sdcard
cp circle/boot/fixup4cd.dat	sdcard
# No longer used, as dispmanx need 32 bits application
#cp circle/boot/config64.txt sdcard/config.txt
cp circle/boot/config.txt sdcard/config.txt
cp circle/boot/armstub7-rpi4.bin sdcard
# No longer used, as dispmanx need 32 bits application
#cp circle/boot/armstub8-rpi4.bin sdcard

echo "*** copy default CART sdcard ***"

if [ ! -d "sdcard/CART" ]; then
  mkdir sdcard/CART
fi
 
echo "*** FONT creation ***"
if [ ! -d "sdcard/FONTS" ]; then
  mkdir sdcard/FONTS
fi

 echo "*** base config creation ***"
if [ ! -d "sdcard/Config" ]; then
  mkdir sdcard/Config
fi

if [ ! -d "sdcard/LAYOUT" ]; then
  mkdir sdcard/LAYOUT
fi

echo "[SETUP]" > config
echo "sync=frame" >> config
echo "cart=SD:/CART/crtc3_projo.cpr" >> config


cp res/101_keyboard sdcard/LAYOUT
cp config sdcard/Config
cp CART/* sdcard/CART
cp gamecontrollerdb.txt sdcard/Config/gamecontrollerdb.txt
cp "res/Facile Sans.ttf" "sdcard/FONTS/Facile Sans.ttf"

