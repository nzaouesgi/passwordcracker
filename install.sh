#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "Please run the installer with root privileges."
   exit 1
fi
echo " "
echo "##########################################################"
echo "#                                                        #"
echo "#          Welcome To Password Cracker V1.0              #"
echo "#                                                        #"
echo "#            Now proceeding to install...                #"
echo "#                                                        #"
echo "#     This software was made in ESGI school (Paris)      #"
echo "#   It is free, but not intended for illegal purposes.   #"
echo "#                                                        #"
echo "##########################################################"
echo " "
sleep 3

apt-get update
apt-get --yes install git
apt-get --yes install libssl1.0.0
apt-get --yes install libssl-dev
apt-get --yes install gcc
apt-get --yes install libgtk-3-0
apt-get --yes install libgtk-3-dev
apt-get --yes install libssh-4
apt-get --yes install libssh-dev
apt-get --yes install libcurl3
apt-get --yes install libcurl4-openssl-dev
apt-get --yes install libsqlite3-0
git clone https://github.com/sqlcipher/sqlcipher.git
cd sqlcipher
./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC -DSQLITE_TEMP_STORE=2" LDFLAGS="-lcrypto"
make
make install
cd ../
export LD_LIBRARY_PATH="/usr/local/lib/"
./build.sh
chmod 777 projetc
echo " "
echo "Installation Done. Type ./projetc to start !"
echo " "
