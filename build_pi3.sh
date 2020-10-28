echo "*** Build target for Raspberry Pi 3 ***"

# Set arch
echo AARCH = 64							          > Config.mk
echo RASPPI = 3							          >> Config.mk
echo PREFIX64 = aarch64-none-elf-   	>> Config.mk
echo STDLIB_SUPPORT = 1					      >> Config.mk
echo DEFINE = -DARM_ALLOW_MULTI_CORE	>> Config.mk
echo CHECK_DEPS = 0                   >> Config.mk

cp Config.mk circle/.
./build_circle.sh


# check output dir
make clean
make 

# copy to target directory
if [ ! -d "sdcard" ]; then
  mkdir sdcard
fi

cp kernel8.img sdcard/pi3


echo "*** End ***"
