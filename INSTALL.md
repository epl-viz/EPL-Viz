# Ubuntu 16.04

:warning: Uninstalling all system wireshark versions is highly recommended! :warning:

## Installing packets:

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update

sudo apt install cmake git gcc-6 g++-6
sudo apt install qt5-default libqt5svg5-dev qtmultimedia5-dev qttools5-dev-tools qtcreator extra-cmake-modules libkf5texteditor-dev
sudo apt install libglib2.0-dev libkrb5-dev flex liblua5.2-dev zlib1g-dev bison libxml2-dev libgeoip-dev
sudo apt install python3.5-dev python3-pip

pip3 install cython
```

## Setting up build environment

```bash
BUILD_ROOT=$HOME # or any other path

# where EVERYTHING should be installed to; should be a path where the user can write to
PREFIX=/opt/EPL

[ ! -d "$BUILD_ROOT" ] && mkdir -p "$BUILD_ROOT"
```

## Building Wireshark

### Building libpcap

```bash
cd "$BUILD_ROOT"
wget http://www.tcpdump.org/release/libpcap-1.8.1.tar.gz
tar -xf libpcap-1.8.1.tar.gz
cd libpcap-1.8.1
./configure --prefix="${PREFIX}"
make
make install
```

### Building Wireshark

```bash
cd "$BUILD_ROOT"
git clone https://github.com/epl-viz/wireshark.git
cd wireshark
git submodule update --init --recursive

mkdir build
cd build
cmake -DPCAP_HINTS="${PREFIX}" -DCMAKE_INSTALL_PREFIX="${PREFIX}" ..
make -j$(nproc)
make install # may require root

# Install additional files not covered with make install
cp ./run/libcapchild.a ${PREFIX}/lib
cp ./run/libcaputils.a ${PREFIX}/lib
cd ..
mkdir -p "${PREFIX}/include/wireshark"
find . -name "*.h" ! -path "*build*" -exec cp --parents {} "${PREFIX}/include/wireshark" \;
```

## Building TinyXML2 version 4

```bash
cd "$BUILD_ROOT"
git clone https://github.com/leethomason/tinyxml2
cd tinyxml2
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="${PREFIX}" ..
make -j$(nproc)
make install
```

## Building QWT5 for qt5

```bash
cd "$BUILD_ROOT"
wget http://downloads.sourceforge.net/qwt/qwt-6.1.3.tar.bz2
tar -xf qwt-6.1.3.tar.bz2
cd qwt-6.1.3
sed -i 's/^.*local\/qwt-.*$//g' qwtconfig.pri
sed -i 's/QWT_CONFIG[^Q]*QwtDesigner//g' qwtconfig.pri
sed -i "s/\$\$\[QT_INSTALL_PREFIX\]/${PREFIX//\//\\/}/g" qwtconfig.pri
qmake qwt.pro
make -j$(nproc)
make install
```

# Builing EPL-Viz

## Building EPL_DataCollect

```bash
cd "$BUILD_ROOT"
git clone https://github.com/epl-viz/EPL_DataCollect
cd EPL_DataCollect
git submodule update --init --recursive
mkdir build
cd build
cmake \
  -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
  -DWireshark_DIR="${PREFIX}/lib/wireshark" \
  -DPCAP_HINTS="${PREFIX}" \
  -DTinyXML2_ROOT="${PREFIX}" \
  -DCMAKE_C_COMPILER=$(which gcc-6) \
  -DCMAKE_CXX_COMPILER=$(which g++-6) ..
make -j$(nproc)
make install
cp "${PREFIX}/bin/dumpcap" ./bin
```

## Building EPL-Viz

```bash
cd "$BUILD_ROOT"
git clone https://github.com/epl-viz/EPL-Viz
cd EPL-Viz
git submodule update --init --recursive
mkdir build
cd build
cmake \
  -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
  -DUSE_SYSTEM_PACKETS=ON \
  -DWireshark_DIR="${PREFIX}/lib/wireshark" \
  -DPCAP_HINTS="${PREFIX}" \
  -DQWT_ROOT="${PREFIX}" \
  -DTinyXML2_ROOT="${PREFIX}" \
  -DPYTHON_EXECUTABLE=/usr/bin/python3 \
  -DCMAKE_C_COMPILER=$(which gcc-6) \
  -DCMAKE_CXX_COMPILER=$(which g++-6) ..
make -j$(nproc)
make install
cp "${PREFIX}/bin/dumpcap" ./bin
```

### :warning: NOTE:

You have to give dumpcap special privileges to enable live captureing!
You can use `sudo setcap cap_net_admin,cap_net_raw+eip "${PREFIX}/bin/dumpcap"` for instance.
