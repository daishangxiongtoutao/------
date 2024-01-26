How to expand Wine on a Ubuntu System for FAST ComTec USB devices
=================================================================

This folder contains compiled DLLs wusb3api for 
wine on Ubuntu 18.04 that work also on ubuntu 20.04.
First install 64-bit and 32-bit wine on Ubuntu with standard commands
on a terminal:

sudo apt install wine64
sudo apt install wine32

It is neccessary that libusb-1.0 is installed:

sudo apt-get install libusb-1.0-0-dev

Try to find wuapi.* using commands
cd /
find -name wuapi.*

The 32-bit DLLs for Wine should be in
/usr/lib/i386-linux-gnu/wine/

and the fake DLLs in
/usr/lib/i386-linux-gnu/wine/fakedlls/

The 64-bit DLLs for Wine should be in
/usr/lib/x86_64-linux-gnu/wine/

and the 64-bit fake DLLs in
/usr/lib/x86_64-linux-gnu/wine/fakedlls/

Now copy the 32-bit and 64-bit and wusb3api.dll.so
into the corresponding folders,
and also the wusb3api.dll.fake
into the fake dll folders:

cd ~/Downloads/mcs8/mcs8driver/linux/ubuntu-18.04/wine32/wusb3api
sudo cp wusb3api.dll.so /usr/lib/i386-linux-gnu/wine/
sudo cp wusb3api.dll.fake /usr/lib/i386-linux-gnu/wine/fakedlls/
cd ../../wine64/wusb3api
sudo cp wusb3api.dll.so /usr/lib/x86_64-linux-gnu/wine/
sudo cp wusb3api.dll.fake /usr/lib/x86_64-linux-gnu/wine/fakedlls/

Rename the fake dlls from wusb3api.dll.fake into wusb3api.dll
Then change the access restrictions using commands like

cd /usr/lib/x86_64-linux-gnu/wine/fakedlls/
sudo mv wusb3api.dll.fake wusb3api.dll
sudo chmod 644 wusb3api.dll
cd ..
sudo chmod 644 wusb3api.dll.so

cd /usr/lib/i386-linux-gnu/wine/fakedlls/
sudo mv wusb3api.dll.fake wusb3api.dll
sudo chmod 644 wusb3api.dll
cd ..
sudo chmod 644 wusb3api.dll.so

Now install the 32- and 64-bit mcs8 software using commands

wine setup.exe
wine setupx64.exe

the 32-bit software will be installed in
~.wine/drive_c/MCS8


the 64-bit software will be installed in
~.wine/drive_c/mcs8x64

Before you start the software, type lsusb to see the USB devices. 
The line showing the MCS8 looks like

Bus 001 Device 006: ID 0464:00f4 Cypress Semiconductor Corp. FX3

You see here that the device is at Bus 001 and has device number 6.
Now type

sudo chmod 666 /dev/bus/usb/001/006

This enables access to the device by all users.

