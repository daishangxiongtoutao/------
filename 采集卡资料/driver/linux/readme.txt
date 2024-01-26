How to Use the MPANT software for MCS8 under Linux
===================================================
(c) 2017-2020 FAST ComTec GmbH 

The Linux software uses the libusb-1.0 software. No driver installation
is required, only the libusb-1.0 package.

General hints:
===================================================

Install the libusb-1.0 package and connect the MCS8 module.
Start a console like XTerm and type lsusb. The line showing the
MCS8 looks like

Bus 007 Device 002: ID 04b4:00f4 Cypress Semiconductor Corp.

If you don't see the USB device with ID 004b4:00f4, please remove 
the power from the device, reconnect it and try it again.
You see that the device is at Bus 007 and has device number 2. 
Now type su root and enter the root password. Then type

chmod 666 /dev/bus/usb/007/002

This enables access of the device by all users. Type su yourname
to leave root and return to your normal privileges.

The Wine Windows Emulator can be expanded by a DLL wusb3api.dll
so that the Windows MPANT software runs under wine and 
can operate the hardware. 
It was tested with Debian 8 as well with the 32 bit and the 
64 bit version. 

Here is a guide how to get it working:

from http://ftp.winehq.org/pub/wine/source download the latest
wine source, say the file wine-2.5.tar.xz, put it in a local
folder, say winesrc. Using the command 

tar xpvf wine-2.5.tar.xz

extract the wine source files into the local folder winesrc/wine-2.5

Now go to the dlls subfolder, make a new subdirectory wusb3api and copy
the supplied wusb3api source files Makefile.in, wusb3api.c, wusb3api.h and 
wusb3api.spec into that directory. 

Then edit the file configure.ac in winesrc/wine-2.5. Search
for the characters "wuapi". After the line

WINE_CONFIG_DLL(wuapi,,[clean])

enter the lines

WINE_CONFIG_DLL(wusb3api,,[clean])

Edit also the file configure. After the line

wine_fn_config_dll wuapi enable_wuapi

enter the lines

wine_fn_config_dll wusb3api enable_wusb3api clean

and after the line

enable_wuapi

enter the lines

enable wusb3api

Now compile and install wine from the source as described in 
wiki.winehq.org. Look here for "Recommended Packages" and
install all the required packages.

The standard commands for compiling and installing wine are 

./configure
make > make.log 2>&1
su root
make install
su username
winecfg

For installing 64-bit Wine with 32-bit support please refer to
https://wiki.winehq.org/Building_Wine#Shared_WoW64

After the configure command probably some warnings will complain about
missing packages. Install then that packages and try the configure
command again.
The complete wine compilation process with "make" takes about 2 hours.

The MPANT software for MCS8 can be installed with the supplied
"SETUPX86.exe" for the 32-bit version or "SETUPX64.exe" for the 64-bit 
version. Restart the PC before starting the software for the first time.

It runs also under Windows. Under Windows the supplied fastusb3 driver 
must be installed.


