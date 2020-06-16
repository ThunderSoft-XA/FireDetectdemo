#!/bin/bash

cp thirdparty/opencv-3.3.1.zip ~/
update net ssh vim opencl
sudo apt-get update -y
sudo apt-get upgrade -y
echo "system update successful!!!!!!!!!"

sudo apt-get install net-tools -y
sudo apt-get install openssh-server -y
sudo apt-get install vim -y
sudo apt-get install ocl-icd-opencl-dev -y
echo "tools update successful!!!!!!!!!!"


sudo apt-get install gstreamer1.0-alsa  gstreamer1.0-clutter-3.0  gstreamer1.0-doc  gstreamer1.0-espeak  gstreamer1.0-fluendo-mp3     gstreamer1.0-libav  gstreamer1.0-libav-dbg  gstreamer1.0-nice    gstreamer1.0-packagekit  gstreamer1.0-plugins-bad  gstreamer1.0-plugins-bad-dbg  gstreamer1.0-plugins-bad-doc  gstreamer1.0-plugins-base  gstreamer1.0-plugins-base-apps  gstreamer1.0-plugins-base-dbg  gstreamer1.0-plugins-base-doc  gstreamer1.0-plugins-good  gstreamer1.0-plugins-good-dbg  gstreamer1.0-plugins-good-doc  gstreamer1.0-plugins-ugly  gstreamer1.0-plugins-ugly-dbg  gstreamer1.0-plugins-ugly-doc  gstreamer1.0-pocketsphinx  gstreamer1.0-pulseaudio   gstreamer1.0-tools  gstreamer1.0-x  gstreamer-qapt -y

sudo apt-get install libgstreamer-plugins-* -y

sudo apt-get install libgstreamer1.0* -y

echo "gstream install successful!!!!!!"



sudo apt-get install gcc-4.9 -y
ls /usr/bin/gcc* 
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100
sudo update-alternatives --config gcc
gcc -v
sudo apt-get install g++-4.9 -y
ls /usr/bin/g++* 
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100
sudo update-alternatives --config g++
g++ -v

echo "javaJDK install successful!!!!!!"

#cd $HOME
#sudo apt-get install -y \
    cmake \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libeigen3-dev \
    libglew-dev \
    libgtk2.0-dev \
    libgtk-3-dev \
    libjasper-dev \
    libjpeg-dev \
    libpng12-dev \
    libpostproc-dev \
    libswscale-dev \
    libtbb-dev \
    libtiff5-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libx265-dev \
    zlib1g-dev \
    pkg-config

wget https://github.com/opencv/opencv/archive/3.3.1.zip -O opencv-3.3.1.zip
unzip opencv-3.3.1.zip

cd opencv-3.3.1
mkdir -p build/
cd build
sudo mkdir -p /usr/local/opencv
sudo chmod a+wrx -R /usr/local/opencv/

time cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local/opencv \
      -D ENABLE_FAST_MATH=ON \
      -D WITH_CUBLAS=ON \
      -D WITH_LIBV4L=ON \
      -D WITH_GSTREAMER=ON \
     -D WITH_GSTREAMER_0_10=OFF \
      ../

make -j8

make install











