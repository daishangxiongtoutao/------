/* wusb3api.c */
/* Basic Wine wrapper */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <windef.h>
#include <libusb-1.0/libusb.h>
#include "wusb3api.h"

#define VENDORCMD_TIMEOUT	(5000)		// Timeout (in milliseconds) for each vendor command.


int WINAPI DllMain(void *hInst, unsigned int ul_reason_being_called, void *lpReserved)
{
    return 1;
//        UNREFERENCED_PARAMETER(hInst);
//        UNREFERENCED_PARAMETER(ul_reason_being_called);
//        UNREFERENCED_PARAMETER(lpReserved);
}

int inidevlist(USBCONTEXT *myusb)
{
  int cnt, i=0, r, desclen;
  libusb_device *dev0;
  unsigned char data[1024];

  cnt = (int)libusb_get_device_list(NULL, &myusb->devs);
  if (cnt < 0)
	return (int) cnt;
  myusb->devices = 0;
  while ((dev0 = myusb->devs[i++]) != NULL && (myusb->devices < 6)) {
	struct libusb_device_descriptor desc;
    myusb->hMpa[myusb->devices] = NULL;
	r = libusb_get_device_descriptor(dev0, &desc);
	if (r < 0) {
//  	  fprintf(stderr, "failed to get device descriptor");
//      libusb_free_device_list(devs, 0);
	  return -1;
	}
	if ((desc.idVendor == 0x04B4) && (desc.idProduct == 0x00F4)) {
	  r = libusb_open(dev0, &myusb->hMpa[myusb->devices]);
	  if (!r) {
	    r=libusb_get_descriptor(myusb->hMpa[myusb->devices], LIBUSB_DT_CONFIG, 0, data, 0x09);
		if (r > 0) {
		  desclen = data[2] + (data[3] << 8);
          r=libusb_get_descriptor(myusb->hMpa[myusb->devices], LIBUSB_DT_CONFIG, 0, data, desclen);
		  if (desclen == 0x39) {
		    myusb->board[myusb->devices].endpoints = data[13];
		    myusb->board[myusb->devices].maxpacket = data[22] + (data[23]<<8);
		    myusb->board[myusb->devices].maxpacket1 = data[35] + (data[36]<<8);
	        myusb->board[myusb->devices].burst1 = data[40]+1;
		    myusb->board[myusb->devices].maxpacket2 = data[48] + (data[49]<<8);
	        myusb->board[myusb->devices].burst2 = data[53]+1;
		  }
		  else if (desclen == 0x27) {
		    myusb->board[myusb->devices].endpoints = data[13];
		    myusb->board[myusb->devices].maxpacket = data[22] + (data[23]<<8);
		    myusb->board[myusb->devices].maxpacket1 = data[29] + (data[30]<<8);
	        myusb->board[myusb->devices].burst1 = 1;
		    myusb->board[myusb->devices].maxpacket2 = data[36] + (data[37]<<8);
	        myusb->board[myusb->devices].burst2 = 1;
		  }
		}
		else {
		  myusb->board[myusb->devices].endpoints = 3;
	      myusb->board[myusb->devices].burst1 = 16;
	      myusb->board[myusb->devices].burst2 = 16;
		}
        r = libusb_set_auto_detach_kernel_driver(myusb->hMpa[myusb->devices], 1);
        r = libusb_claim_interface(myusb->hMpa[myusb->devices], 0);
        if (!r) {
          myusb->devices++;
        }
	  }
	}
  }
  if (!(myusb->devices)) return -1;
  return cnt;
}

void waitmsec(int timeout)
{
  struct timeval stimebuffer;      // start
  struct timeval etimebuffer;      // end
  struct timezone tz;
  int stime;                    // start time in millisec + wait
  int etime;                    // akt time in millisec

  gettimeofday(&stimebuffer, &tz);
  stime = 1000L * stimebuffer.tv_sec + (int)(stimebuffer.tv_usec/1000) + (int)timeout;

  do {
	 gettimeofday(&etimebuffer, &tz);
	 etime = 1000L * etimebuffer.tv_sec + (int)(etimebuffer.tv_usec/1000);
  } while (stime > etime);
}

int fpgadwldbin(
		libusb_device_handle  *h,
		int len, 
		unsigned char *buf
		)
{
  int ret=-1, i;
  int transferred=0;

  unsigned char cbuf[16];
  cbuf[0] = (unsigned char)(len & 0x000000FF);
  cbuf[1] = (unsigned char)((len & 0x0000FF00) >> 8);
  cbuf[2] = (unsigned char)((len & 0x00FF0000) >> 16);
  cbuf[3] = (unsigned char)((len & 0xFF000000) >> 24);
  for (i=4; i<16; i++)
	  cbuf[i] = (unsigned char)0;
  ret = libusb_control_transfer (h, 0x40, 0xB2, 0, 0,
				cbuf, 16, VENDORCMD_TIMEOUT);
  ret = libusb_bulk_transfer(h, 0x01, buf, len, &transferred, 30000);
  if ( ret == 0 ) {

    waitmsec(200);
    cbuf[0] = 0;
    ret = libusb_control_transfer (h, 0xC0, 0xB1, 0, 0,
				cbuf, 16, VENDORCMD_TIMEOUT);

    if (cbuf[0] == (unsigned char)1) {	// configuration successful
      ret = 0;
    }
    else {				// configuration failed
      ret = -2;
    }
  }
  else {				// failed to send configuration data to FPGA
    ret = -3;
  }
  return ret;
}

int loadfpgafw(libusb_device_handle *h, char *filename)
{
//  clock_t c;
  int fd, Errset=0;
  unsigned int len=0, i, len1;
  unsigned char *buf;
  struct stat filestat;

  if (stat (filename, &filestat) != 0) {
//    	  fprintf (stderr, "Error: Failed to stat file %s\n", filename);
    	  return -1;
  }
 
  len = filestat.st_size;
  if (!len) 
    len1 = 0;
  else {
    len1 = len >> 10;
    len1 <<= 10;
    len1 += 1024;
  }
  buf = (unsigned char *)calloc (1, len1);
  if (buf == 0) {
//		fprintf (stderr, "Error: Failed to allocate buffer to store fpga firmware binary\n");
		return -1;
  }
	fd = open (filename, O_RDONLY);
	if (fd < 0) { 
//		fprintf (stderr, "Error: File not found\n");
		return -2;
	}
	read (fd, buf, len);

	close (fd);
  for (i=len; i<len1; i++)
    buf[i] = 0xff;
  
//  printf("Downloading %s...\n", filename);
//  c = clock();
  Errset = fpgadwldbin(h, len1, buf);
  if (Errset) {
//	  printf("Failed! Error=%d\n", Errset);
          free(buf);
	  waitmsec(3000);
	  return -100 * len + Errset;
  }
  free(buf);
  return 0;
}

void WINAPI pfxclose(int nDev, USBCONTEXT *myusb)
{
  int i;    
  if (nDev >= myusb->devices) return;
  libusb_release_interface(myusb->hMpa[nDev], 0);
  libusb_close(myusb->hMpa[nDev]);
  myusb->hMpa[nDev] = NULL;
  myusb->devices--;
  for (i=nDev; i<myusb->devices; i++) {
	myusb->hMpa[i] = myusb->hMpa[i+1];
	myusb->hMpa[i+1] = NULL;
  }
  if (!myusb->devices) {
    libusb_free_device_list(myusb->devs, 0);
    libusb_exit(NULL);
  }
}

int WINAPI pfxopen(int nDev, USBCONTEXT *myusb)
{
  int j, r=0;
//  int success=0;
  char cbuf[16]={'\0'};
  if (!myusb->devices) {
    r = libusb_init(NULL);
	r = inidevlist(myusb);
	if (r < 0) return r;
  }
  if (nDev >= myusb->devices) return -2;
//  success=1;
  myusb->board[nDev].wrtimeout=500;
  myusb->board[nDev].rdtimeout=500;
  myusb->board[nDev].in2timeout=500;
  r = libusb_control_transfer (myusb->hMpa[nDev], 0xC0, 0xB0, 0, 0, (unsigned char *)cbuf, 8, VENDORCMD_TIMEOUT);
//    printf("FW ID=%s\n", cbuf);
  if ((r == 8) && !strncmp(cbuf,"FX3 I2C", 7)) {
//    success = 2;
    r = loadfpgafw(myusb->hMpa[nDev], (char *)(myusb->fpgafwfile));
    if (!r) {
//      success = 3;
	  for (j=0; j<myusb->devices; j++) {
	    r = libusb_release_interface(myusb->hMpa[j], 0);
        libusb_close(myusb->hMpa[j]);
	  }
      libusb_free_device_list(myusb->devs, 0);
      r = inidevlist(myusb);
	  if (r < 0)
//		return (r - 10 * success);
	    return r;
      r = libusb_control_transfer (myusb->hMpa[nDev], 0xC0, 0xB0, 0, 0, (unsigned char *)cbuf, 8, VENDORCMD_TIMEOUT);
//      printf("FW ID=%s\n", cbuf);
    }
    else
//      return (r - 10 * success);
      return r;
  }
  if ((r == 8) && !strncmp(cbuf,"FC-FW 1", 7)) {
//    success = 4;
	  if (myusb->board[nDev].endpoints == 3) {
//	  printf("maxpacket2=%d, burst2=%d\n",board[0].maxpacket2,board[0].burst2);  
      return (int)myusb->hMpa[nDev];
    }
    else {
      r = libusb_release_interface(myusb->hMpa[nDev], 0);
      libusb_close(myusb->hMpa[nDev]);
	  myusb->hMpa[nDev] = NULL;
//	  for (j=i; j<devices; j++)
//		dev[j] = dev[j+1];

//	  myusb->devices--;
      return -1;
    }
  }
  else {
	pfxclose(nDev, myusb);
//	return (-10 * success);
	return -1;
  }
  
//  return (int)myusb->hMpa[nDev];
}

int WINAPI pwritefx(int nDev, USBCONTEXT *myusb, unsigned char *data, unsigned int length, int *written)
{
	int ret=0;
    ret = libusb_bulk_transfer(myusb->hMpa[nDev], 0x01, (unsigned char *)data, length, 
		written, myusb->board[nDev].wrtimeout);
	return ret;
}

int WINAPI preadfx(int nDev, USBCONTEXT *myusb, fx_read_buf *A, int *result)
{
    int ret=0;
    ret = libusb_bulk_transfer(myusb->hMpa[nDev], 0x81, (unsigned char *)A->buf, A->count, 
		result, myusb->board[nDev].rdtimeout);
	return ret;
}

int WINAPI preadfxbuf(int nDev, USBCONTEXT *myusb, unsigned char *buf, int size, int *result, int timeout)
{
  int ret;
  ret = libusb_bulk_transfer(myusb->hMpa[nDev], 0x82, buf, size, result, timeout);
  return ret;
}

int WINAPI preadeeprom(
		int nDev, USBCONTEXT *myusb, 
		int address, int len, unsigned char *buf)
{
	// read from I2C EEPROM. The page size is fixed to 64 bytes.
	// maximum request length is 4 KB.
  int ret=-1;
  ret = libusb_control_transfer (myusb->hMpa[nDev], 0xC0, 0xBB, 
	  (unsigned short)((address >> 16) & 0x07), 
	  (unsigned short)address, buf, (unsigned short)len, VENDORCMD_TIMEOUT);
  return ret;
}

int WINAPI pwriteeeprom(int nDev, USBCONTEXT *myusb, int address, int len, unsigned char *buf)
{
	// write to I2C EEPROM. The page size is fixed to 64 bytes.
	// maximum request length is 4 KB.
  int ret=-1;
  ret = libusb_control_transfer (myusb->hMpa[nDev], 0x40, 0xBA, (unsigned short)((address >> 16) & 0x07), 
	  (unsigned short)address, buf, (unsigned short)len, VENDORCMD_TIMEOUT);
  return ret;
}

