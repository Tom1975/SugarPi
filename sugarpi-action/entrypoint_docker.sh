#!/bin/sh -l
echo "PATH : $(PATH)"
echo "who-to-build value : $1"

chmod +x /build_all.sh
chmod +x /build_circle.sh
chmod +x /build_pi3.sh
chmod +x /build_pi4.sh

./build_all.sh
