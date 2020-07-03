#!/bin/bash

# Install PATH
INSTALL_PATH="${HOME}/Documents/KangSK"

# Install packages
sudo apt update && sudo apt upgrade
sudo apt install build-essential  scons zlib1g-dev m4 swig python-pip python-virtualenv

# Make python environment
mkdir $HOME/.pyenv; cd $HOME/.pyenv
sudo virtualenv -p "/usr/bin/python2.7" gem5 &&
source "gem5/bin/activate" &&
which python &&
sudo pip install six scons

