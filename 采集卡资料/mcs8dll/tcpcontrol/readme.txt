FAST ComTec New Control software for MCS6A, MCA4(A), MPA4 and MCS8(A)

Nov-19-2021
The sourcecode of the wmcsctrl program is now part of the MCS8 DLL software.

Nov-11-2021
- Following new commands are now implemented in the server programs 
MCS6A, MCA4, MCA4A, MPA4, MCS8:
sendconf 0		; sends the configuraton of module 0, the format is
			; like the header of a .mpa file
senddat disp start   	; sends 256 data of spectrum #disp beginning 
			; from channel #start in a CSV format 
			; channel<tab>value if value is not zero.

Oct-15-2021 first version
- It is necessary that the external control licence is enabled in the Licence Key.
Otherwise it works only if the local PC has no connection to the hardware, or
message boxes will appear at the local PC showing the message sent over network
and indicating demo mode.

- Copy the programs tcpserv.exe and udpserv.exe to the local PC into the folder 
containing the operating software, for example C:\MCS6Ax64.

- On the remote Windows PC that should control the device over network, create
a folder like C:\Control and copy the files wmcsctrl.exe and wmcslib.dll
into it.

- Start the udpserv.exe program at the local PC, preferably from a command prompt. 
If there arises a message asking to unblock the network access, click to allow the access.

- On the remote PC start the wmcsctrl program from a command prompt. 
If there arises a message asking to unblock the network access, click to allow the access.

- If the connection is found and established, the command window will show a short
help with a list of commands:

H		Help
winexec program	run Windows program
end		end connection
bye		exit tcp server program
Q		Quit and terminate tcp server
?		Show Status
exit		exit MPANT server program
start		Start acquisition
halt		Halt acquisition
erase		Erase spectra
(... more see command language in the manual)

Otherwise a message "no boards found" appears and the program terminates. 
In this case terminate and restart the udpserv program and also the 
wmcsctrl program. Once the connection has worked, a file wmcs.ini is 
written into the control folder at the remote PC, containing the ip address 
of the local PC. From now on start the tcpserv.exe program at the local PC and 
not udpserv.exe. The wmcs.ini file looks like

localpcs=1
ip=192.168.178.66

- It is possible to start and exit the MPANT server program from the remote PC using
commands like

winexec MCS6A
exit

- It is also possible to control several local PCs from a remote PC, the commands
are then send to all local PCs.

