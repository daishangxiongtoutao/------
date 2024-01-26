typedef struct fx_read_buf {
  int count;
  unsigned char buf[64];
} fx_read_buf; 

typedef struct {
  int endpoints;
  int maxpacket;
  int maxpacket1;
  int maxpacket2;
  int burst1;
  int burst2;
  int rdtimeout;
  int wrtimeout;
  int in2timeout;
} USBDEV;

typedef struct {
  int devices;
  libusb_device **devs;
  libusb_device_handle *hMpa[6];
  USBDEV board[6];
  char fpgafwfile[200];  
} USBCONTEXT;

int WINAPI fxopen(int nDev, USBCONTEXT *myusb);
void WINAPI fxclose(int nDev, USBCONTEXT *myusb);
int WINAPI writefx(int nDev, USBCONTEXT *myusb, unsigned char *data, unsigned int length, int *written);
int WINAPI readfx(int nDev, USBCONTEXT *myusb, fx_read_buf *A, int *result);
int WINAPI readfxbuf(int nDev, USBCONTEXT *myusb, unsigned char *buf, int size, int *result, int timeout);
int WINAPI readeeprom(int nDev,	USBCONTEXT *myusb, int address, int len, unsigned char *buf);
int WINAPI writeeeprom(int nDev, USBCONTEXT *myusb, int address, int len, unsigned char *buf);
