#./build_pi3.sh
# No longer used, as dispmanx need 32 bits application
#./build_pi4.sh
./build_pi4_32bits.sh
retn_code=$?

if [ $retn_code -eq 0 ];then
  echo "Pi4 32bits built correctly"
else
  echo "*** ERROR BUILDING PI4_32 version !!"
  exit -1
fi

echo "*** copy boot to sdcard ***"
cd circle-stdlib/libs/circle/boot
make 
cd ../../../..

cp circle-stdlib/libs/circle/boot/bootcode.bin sdcard
cp circle-stdlib/libs/circle/boot/start.elf sdcard
cp circle-stdlib/libs/circle/boot/start4.elf sdcard
cp circle-stdlib/libs/circle/boot/fixup.dat sdcard
cp circle-stdlib/libs/circle/boot/fixup4.dat sdcard
cp circle-stdlib/libs/circle/boot/bcm2711-rpi-4-b.dtb sdcard
cp circle-stdlib/libs/circle/boot/bcm2711-rpi-400.dtb sdcard
cp circle-stdlib/libs/circle/boot/bcm2711-rpi-cm4.dtb sdcard
cp circle-stdlib/libs/circle/boot/bcm2712-rpi-5-b.dtb sdcard
cp circle-stdlib/libs/circle/boot/bcm2710-rpi-zero-2-w.dtb sdcard
cp circle-stdlib/libs/circle/boot/LICENCE.broadcom	sdcard
cp circle-stdlib/libs/circle/boot/COPYING.linux sdcard
cp circle-stdlib/libs/circle/boot/start_cd.elf	sdcard
cp circle-stdlib/libs/circle/boot/start4cd.elf	sdcard
cp circle-stdlib/libs/circle/boot/fixup_cd.dat	sdcard
cp circle-stdlib/libs/circle/boot/fixup4cd.dat	sdcard
# No longer used, as dispmanx need 32 bits application
#cp circle-stdlib/libs/circle/boot/config64.txt sdcard/config.txt
cp circle-stdlib/libs/circle/boot/config.txt sdcard/config.txt
cp circle-stdlib/libs/circle/boot/armstub7-rpi4.bin sdcard
# No longer used, as dispmanx need 32 bits application
#cp circle-stdlib/libs/circle/boot/armstub8-rpi4.bin sdcard

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

if [ ! -d "sdcard/ROMS" ]; then
  mkdir sdcard/ROMS
fi

if [ ! -d "sdcard/TAPE" ]; then
  mkdir sdcard/TAPE
fi

if [ ! -d "sdcard/DISK" ]; then
  mkdir sdcard/DISK
fi

if [ ! -d "sdcard/RES" ]; then
  mkdir sdcard/RES
fi

if [ ! -d "sdcard/AMSTRAD" ]; then
  mkdir sdcard/AMSTRAD
fi

echo "[SETUP]" > config
echo "sync=frame" >> config
echo "cart=SD:/CART/crtc3_projo.cpr" >> config


cp res/101_keyboard sdcard/LAYOUT
cp config sdcard/Config
cp CART/* sdcard/CART
cp gamecontrollerdb.txt sdcard/Config/gamecontrollerdb.txt
cp "res/Facile_Sans.ttf" "sdcard/FONTS/Facile_Sans.ttf"
cp "res/logo.bin" "sdcard/RES/logo.bin"
cp "res/Img464.bin" "sdcard/RES/Img464.bin"
cp "res/Img664.bin" "sdcard/RES/Img664.bin"
cp "res/Img6128.bin" "sdcard/RES/Img6128.bin"
cp "res/ImgGx4000.bin" "sdcard/RES/ImgGx4000.bin"
cp "res/Img464_plus.bin" "sdcard/RES/Img464_plus.bin"
cp "res/Img6128_plus.bin" "sdcard/RES/Img6128_plus.bin"
cp "res/ROMS/*" "sdcard/ROMS"
cp "res/AMSTRAD/*" "sdcard/AMSTRAD"
cp "res/labels.ini" "sdcard/RES/labels.ini"
