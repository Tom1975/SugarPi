echo "*** Build target for Raspberry Pi 2 ***"

# Build Circle++
echo "*** Build Circle++ for Raspberry Pi 2 ***"

echo RASPPI = 2 			 	> Config.mk
echo PREFIX = arm-none-eabi- 	>> Config.mk
echo STDLIB_SUPPORT = 1			>> Config.mk

cp Config.mk circle/.
./build_circle.sh

# check output dir
make clean
make 

# copy to target directory
if [ ! -d "sdcard" ]; then
  mkdir sdcard
fi
cp kernel7.img sdcard/.

echo "*** End ***"