#!/bin/bash

# Install PATH
INS_PATH=$(pwd)

# Install packages
sudo apt update && sudo apt upgrade
sudo apt install build-essential  scons zlib1g-dev m4 swig python-pip python-virtualenv

# Make python environment
mkdir $HOME/.pyenv; cd $HOME/.pyenv
sudo virtualenv -p "/usr/bin/python2.7" --system-site-packages gem5 &&
sudo chmod 0777 "$HOME/.pyenv/gem5" &&
source "gem5/bin/activate" &&
pip install six scons

# Build
cd $INS_PATH/gem5
scons build/ARM/gem5.opt -j $(nproc) --ignore-style
