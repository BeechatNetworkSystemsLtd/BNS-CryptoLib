#!/bin/bash

apt-get update
apt-get install -y git
apt-get install -y bc
apt-get install -y bison
apt-get install -y flex
apt-get install -y libssl-dev
apt-get install -y libncurses5-dev
wget https://raw.githubusercontent.com/notro/rpi-source/master/rpi-source -O /usr/local/bin/rpi-source && sudo chmod +x /usr/local/bin/rpi-source && /usr/local/bin/rpi-source -q --tag-update
rpi-source
make -C ./clock_module

