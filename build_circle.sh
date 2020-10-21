echo "*** Build Circle ***"

cp Rules.mk circle
cd circle

./makeall clean
./makeall --nosample

cd addon/linux
make clean
make

cd ../SDCard
make clean
make

cd ../fatfs
make clean
make

cd ../vc4/sound
make clean
make

cd ../vchiq
make clean
make

cd ../../boot
make clean
make

echo "*** End of Circle build ***"
