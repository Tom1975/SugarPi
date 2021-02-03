./build_pi3.sh
./build_pi4.sh

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
cp circle/boot/config64.txt sdcard/config.txt

echo "*** copy default CART sdcard ***"

if [ ! -d "sdcard/CART" ]; then
  mkdir sdcard/CART
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
