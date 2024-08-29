echo "*** Build target for Raspberry Pi 4 ***"

# Set arch
#echo ARCH = 32                        > Config.mk
#echo FLOAT_ABI = hard                 >> Config.mk
#echo RASPPI = 4							          >> Config.mk
#echo PREFIX = arm-none-eabi-          >> Config.mk
#echo STDLIB_SUPPORT = 3					      >> Config.mk
#echo DEFINE = -DARM_ALLOW_MULTI_CORE	>> Config.mk
#echo CHECK_DEPS = 0                   >> Config.mk

cp Config.mk circle/.
cp Config.mk  circle-stdlib/.
./build_circle.sh

# check output dir
make clean
make 

# copy to target directory
if [ ! -d "sdcard" ]; then
  mkdir sdcard
fi
cp kernel7l.img sdcard/.

echo "*** End ***"
