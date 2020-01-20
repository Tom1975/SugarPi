echo "*** Build target for Raspberry Pi 3 ***"

# Set arch
echo AARCH = 64							> Config.mk
echo RASPPI = 3							>> Config.mk
echo PREFIX64 = aarch64-elf-			>> Config.mk
echo STDLIB_SUPPORT = 1					>> Config.mk
echo DEFINE = -DARM_ALLOW_MULTI_CORE	>> Config.mk
echo POSTFIX = ".exe" 					>> Config.mk

cp Config.mk circle/.
./build_circle.sh


# check output dir
make clean
make 

# copy to target directory
if [ ! -d "sdcard" ]; then
  mkdir sdcard
fi
cp kernel8.img sdcard/.

echo "*** End ***"
