## 3rdParty repositories

### Dependencies
```bash
sudo apt install libfftw3-dev libfftw3-3 libfftw3-bin  libpng-dev libpng++-dev libpng-tools libpng16-16 kissfft-tools libkissfft-dev libkissfft-float131
```
### kissfft
```bash
git clone https://github.com/mborgerding/kissfft.git
cd kissfft
mkdir -p build
cd build
cmake -DKISSFFT_DATATYPE=int16_t -DKISSFFT_STATIC=ON -DKISSFFT_OPENMP=ON ..
make all
sudo make install
make
```