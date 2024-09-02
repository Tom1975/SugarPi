#!/bin/sh -l

#chmod +x ./build_all.sh
#chmod +x ./build_circle.sh
#chmod +x ./build_pi3.sh
#chmod +x ./build_pi4.sh
chmod +x ./build_pi4_32bits.sh

#./build_all.sh
./build_pi4_32bits.sh
exit $?
