ls -l

echo "who-to-build value : $1"


sudo chmod +x /build_all.sh
sudo chmod +x /build_circle.sh
sudo chmod +x /build_pi3.sh
sudo chmod +x /build_pi4.sh

./build_all.sh
