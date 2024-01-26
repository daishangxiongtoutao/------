#define  WINDOWSNT
#undef  WINDOWS95
#undef  WINDOWS31

#ifdef WINDOWS31

#define GET_WM_COMMAND_ID(w)  w
#define GET_WM_COMMAND_CMD(w,l) HIWORD(l)
#define GET_WM_COMMAND_HWND(l) LOWORD(l)
#define GET_WM_SCRHWND(l) HIWORD(l)
#define GET_WM_SCROLLPOS(w,l) LOWORD(l)
#define FIND_WINDOW(a,b) FindWindow(b,a)
#define HUGE huge
#define USHORT unsigned short
#define SetForegroundWindow(w)
#define APIENTRY FAR PASCAL
#define Sleep(t) waitmsec(t)

#else

#define GET_WM_COMMAND_ID(w)  LOWORD(w)
#define GET_WM_COMMAND_CMD(w,l) HIWORD(w)
#define GET_WM_COMMAND_HWND(l) l
#define GET_WM_SCRHWND(l) l
#define GET_WM_SCROLLPOS(w,l) (short)HIWORD(w)
#define FIND_WINDOW(a,b) FindWindow(a,b)
#define HUGE
#define _fmemcpy memcpy
#define _fstrcpy strcpy

#endif

typedef struct {
        int use;
        int port;
        unsigned int baud;
        int dbits;
        int sbits;
        int parity;
        int echo;
        HWND hwndserver;
        LPSTR cmd;
} COMCTL, far *LPCOMCTL;

#define ST_RUNTIME    0
#define ST_OFLS		  1
#define ST_TOTALSUM   2
#define ST_ROISUM     3
#define ST_ROIRATE    4
#define ST_SWEEPS     5
#define ST_STARTS     6
#define ST_ZEROEVTS	  7

typedef struct{
  unsigned int started; // aquisition status 
  unsigned int maxval;  // maxval
  double cnt[8];        // status: runtime in msec, ofls, 
						// total sum, roi sum, roi rate, sweeps, starts, zeros
} ACQSTATUS;

typedef struct {
  int savedata;         // bit 0: auto save after stop
						 // bit 1: write listfile
						 // bit 2: listfile only, no evaluation
  int autoinc;          // 1 if auto increment filename
  int fmt;              // format type (seperate spectra): 
						 // 0 == ASCII, 1 == binary,
                         // 2 == CSV
  int mpafmt;           // format used in mpa datafiles 
  int sephead;          // seperate Header 
  int smpts;
  int caluse;
  char filename[256];
  char specfile[256];
  char command[256];
} DATSETTING;

typedef struct {
  int use;              // 1 if Replay Mode ON
  int modified;         // Bit 0: 1 if different settings are used
						 // (Bit 1: Write ASCII, reserved)
  int limit;            // 0: all, 
                        // 1: limited sweep range
  int speed;			// replay speed in units of 100 kB / sec
  double startsfrom;    // first start#
  double startsto;      // last start#
  double startspreset;  // last start - first start
  char filename[256];
} REPLAYSETTING;

typedef struct{
  int range;            // spectrum length
  int cftfak;           // LOWORD: 256 * cft factor (t_after_peak / t_to_peak)
						 // HIWORD: max pulse width for CFT 
  int roimin;           // lower ROI limit
  int roimax;           // upper limit: roimin <= channel < roimax
  int nregions;         // number of regions
  int caluse;           // bit0: 1 if calibration used, higher bits: formula
  int calpoints;        // number of calibration points
  int param;            // (reserved:) for MAP and POS: LOWORD=x, HIWORD=y 
  int offset;           // (reserved:) zoomed MAPS: LOWORD: xoffset, HIWORD, yoffset
  int xdim;			//  (reserved:) x resolution of maps
  unsigned int bitshift;  // LOWORD: Binwidth = 2 ^ (bitshift)
						   // HIWORD: Threshold for Coinc
  int active;      // Spectrum definition words for CHN1..8:
                           // active & 0xF  ==0 not used 
                           //               ==1..4 system 1..4
						// bit 8: Enable Tag bits
						// bit 9: start with rising edge 
						// bit 10: time under threshold for pulse width
						// bit 11: pulse width mode for any spectra with both edges enabled
					// Spectrum definition words for calc. spectra:
                           // active & 0xF  ==3 MAP, ((x-xoffs)>>xsh) x ((y-yoffs)>>ysh)
                           //                 ((x-xoffs)>>xsh) x ((y-timeoffs)>>timesh)  
                           //              or ((x-timeoffs)>>timesh x ((y-yoffs)>>ysh)  
						   //         bit4=1: x zoomed MAP
						   //         bit5=1: y zoomed MAP
						   //               ==5 SUM, (x + y)>>xsh
						   //               ==6 DIFF,(x - y + range)>>xsh
  						   //               ==7 ANY, (for compare)
						   //               ==8 COPY, x
						   //               ==9 DLL  fDLL(x,y,z), 
                           //               ==0xA Sweep HISTORY, Sweepnum(x)
// bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
						   // HIWORD(active) = condition no. (0=no condition)
  double eventpreset;   // ROI preset value
  double dummy1;        // (for future use..)
  double dummy2;		// 
  double dummy3;		// 
} ACQSETTING;

typedef struct{
  int range;            // spectrum length
  int cftfak;           // LOWORD: 256 * cft factor (t_after_peak / t_to_peak)
						 // HIWORD: max pulse width for CFT 
  int roimin;           // lower ROI limit
  int roimax;           // upper limit: roimin <= channel < roimax
  int nregions;         // number of regions
  int caluse;           // bit0: 1 if calibration used, higher bits: formula
  int calpoints;        // number of calibration points
  int param;            // (reserved:) for MAP and POS: LOWORD=x, HIWORD=y 
  int offset;           // (reserved:) zoomed MAPS: LOWORD: xoffset, HIWORD, yoffset
  int xdim;			//  (reserved:) x resolution of maps
  unsigned int bitshift;  // LOWORD: Binwidth = 2 ^ (bitshift)
						   // HIWORD: Threshold for Coinc
  int active;      // Spectrum definition words for CHN1..8:
                           // active & 0xF  ==0 not used 
                           //               ==1..8 enabled, system 1..4
						// bit 8: Enable Tag bits
						// bit 9: start with rising edge 
						// bit 10: time under threshold for pulse width
						// bit 11: pulse width mode for any spectra with both edges enabled
					// Spectrum definition words for calc. spectra:
                           // active & 0xF  ==3 MAP, ((x-xoffs)>>xsh) x ((y-yoffs)>>ysh)
  						   //         bit4=1: x zoomed MAP
						   //         bit5=1: y zoomed MAP
						   //               ==5 SUM, (x + y)>>xsh
						   //               ==6 DIFF,(x - y + range)>>xsh
  						   //               ==7 ANY, (for compare)
						   //               ==8 COPY, x
						   //               ==10 SW-HIS, Sweep History 
                           // bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
						   // HIWORD(active) = condition no. (0=no condition)
  double eventpreset;   // ROI preset value
  double dummy1;        // (for future use..)
  double dummy2;		// 
  double dummy3;        // 
						// MPANT or Server private saved settings:
  int type;			// 0=single, 1=MAP, 2=ISO...
  int ydim;			// y resolution of maps
  int reserved[16];
} EXTACQSETTING;

/*
typedef struct{
  int range;            // spectrum length
  int prena;            // bit 0: livetime preset enabled
                         // bit 1: ROI preset enabled
  int roimin;           // lower ROI limit
  int roimax;           // upper limit: roimin <= channel < roimax
  int nregions;         // number of regions
  int caluse;           // bit0: 1 if calibration used, higher bits: formula
  int calpoints;        // number of calibration points
  int param;            // for MAP and POS: LOWORD=x, HIGHWORD=y (rtime=256, RTC=257)
						 // bit 9 of low or high word: time differences, bit 12..15: time ref. parameter
  int offset;           // zoomed MAPS: LOWORD: xoffset, HIGHWORD, yoffset
  int xdim;				// x resolution of maps
  int timesh;			// bitshift for timespectra
						// DLL function z * fDLL(x,y): HIWORD = z parameter
  int active;			// Spectrum definition words for ADC1..16:
                           // active & 0xFF ==0 not used 
                           //               ==1 single
                           //               ==2 coinc with any
						   // bit 4..7 in group 1..4 for ADCs
						// Spectrum definition words for calc. spectra:
                           // active & 0xF  ==3 MAP, ((x-xoffs)>>xsh) x ((y-yoffs)>>ysh)
                           //               ==0xB TIM, MAP with RTC or rtime as x or y
                           //                 ((x-xoffs)>>xsh) x ((y-timeoffs)>>timesh)  
                           //              or ((x-timeoffs)>>timesh x ((y-yoffs)>>ysh)  
						   //         bit4=1: x zoomed MAP
						   //         bit5=1: y zoomed MAP
						   //               ==4 POS, (y<<xsh) /(x + y)
						   //               ==5 SUM, (x + y)>>xsh
						   //               ==6 DIV, (x<<xsh) / y
  						   //               ==7 ANY, (for compare)
						   //               ==8 COPY, x
						   //               ==9 DLL, fDLL(x,y,z)
						   //               ==0xA HISTORY, x
						   //               ==0xC DIFF, (x+range-1-y)>>xsh                           // bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
						   // bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
						   //               ==0xF OR, Sum of counts; 
						   //                 LOWORD(param) is the mask for spectra to sum up,
						   //                 param & 0x80000 use calibration
						   // HIWORD(active) = 1+condition no. (0=no condition)
  double roipreset;      // ROI preset value
  double ltpreset;      // livetime preset value
  double timeoffs;		// offset for time spectra
  double dwelltime;     // binsize for time spectra
						// MPANT or Server private saved settings:
  int type;				// 0=single, 1=MAP, 2=ISO...
  int ydim;				// y resolution of maps
  int reserved[16];
} EXTADCSETTING;
*/

typedef struct {
  int sweepmode;		// sweepmode & 0xF: 0 = normal, 
						// 1=differential (relative to first stop in sweep)
						// 4=sequential
						// 5=seq.+diff (Ch1), bit0 = differential mode
						// 6 = CORRELATIONS
						// 7 = diff.+Corr.
						// 9=differential to stop in Ch2, bit3 = Ch2 ref (diff.mode)
						// 0xF = Corr.+diff (Ch2)
						// bit 4: Softw. Start
						// bit 5: "Don't show" tagbits
						// bit 6: Endless
						// bit 7: Start event generation
						// bit 8: Enable Tag bits
						// bit 9: start with rising edge 
						// bit 10: time under threshold for pulse width
						// bit 11: pulse width mode for any spectra with both edges enabled
						// bit 12: abandon Sweepcounter in Data
						// bit 13: "one-hot" mode with tagbits
						// bit 14: start ref (diff.mode) (MCS8)
						// bit 15: MCS8, MCS8A1: enable start input sampling
                        // bit 16..bit 22 ~(input channel enable) 
                        // MCS8A: bit 16..bit 23 ~(input channel enable) 
						// bit 24: (require data lost bit in data) obsolete
						// bit 25: (don't allow 6 byte datalength) obsolete
						// bit 26: 100 ps time resolution instead of 80 ps
						// bit 27: Folded
						// bit 28: Interleaved
						// bit 29: MCS8A: dont use trigger, use CH1 to start sweep
						// bit 30: Use only 1 decoder
  int prena;			// bit 0: realtime preset enabled
						// bit 1: 
                        // bit 2: sweep preset enabled
                        // bit 3: ROI preset enabled
						// bit 4: Starts preset enabled
                        // bit 5: ROI2 preset enabled
                        // bit 6: ROI3 preset enabled
                        // bit 7: ROI4 preset enabled
                        // bit 8: ROI5 preset enabled
                        // bit 9: ROI6 preset enabled
                        // bit 10: ROI7 preset enabled
                        // bit 11: ROI8 preset enabled
  int cycles;			// for sequential mode
  int sequences;		// 
  int syncout;			// LOWORD: sync out; bit 0..5 NIM syncout, bit 8..13 TTL syncout
						// bit7: NIM syncout_invert, bit15: TTL syncout_invert
						// 0="0", 1=10 MHz, 2=100 MHz, 3=200 MHz, 4=1/(80 ps or 100 ps * 64),
						// 5=1/(80 ps or 100 ps * 32), 6=PLL ref clk, 7=Input 0(Start), 8=Input 1, 9=Input 2, 
						// 10..13=Input 3..7, 15=GO in, 16=Start_of_sweep, 17=Armed,
						// 18=SWEEP_ON, 19=WINDOW, 20=HOLD_OFF, 21=EOS_DEADTIME
						// 22=TIME[0],...,51=TIME[29], 52...63=SWEEP[0]..SWEEP[11]
                        //
  int digio;			// LOWORD: Use of Dig I/O, GO Line:
                        // bit 0: status dig 0..3
                        // bit 1: Output digval and increment digval after stop
                        // bit 2: Invert polarity
						// bit 3: Push-Pull output, not possible
                        // bit 4:  Start with Input Dig 4 
                        // bit 5:  Start with Input GO 
						// bit 8: GOWATCH
						// bit 9: GO High at Start
						// bit 10: GO Low at Stop
						// bit 11: Clear at triggered start
						// bit 12: Only triggered start
  int digval;			// digval=0..255 value for samplechanger
  int dac0;				//  DAC0 value (START) 
						//  bit 16: Start with rising edge
  int dac1;				//  DAC1 value (STOP 1) 
  int dac2;				//  DAC2 value (STOP 2)
  int dac3;				//  DAC3 value (STOP 3)
  int dac4;				//  DAC4 value (STOP 4)
  int dac5;				//  DAC5 value (STOP 5)
						// bit (14,15) of each word: 0=falling, 1=rising, 2=both, 3=both+CFT 
						// bit 17 of each: pulse width mode under threshold
  int fdac;				// (Feature DAC 0..16383 --> 0..2.5V) obsolete
						// (HIWORD of fdac: rxdelay 0..15, )
						// (additional delay to all 6 TOF inputs in units of 3.2 ns)
  int tagbits;          // number of tagbits
  int extclk;			// use external clock
  int periods;			// number of periods in folded mode, sweeplength = range * periods
  int serno;			// serial number
  int ddruse;           // bits[0:1] 1=DDR_USE, 2=DDR_2GB, 3=DDR_4GB
						// bits[2]: usb_usage (0 means demo mode)
						// bits[4:5]: wdlen
  int active;           // module in system
  double holdafter;	    // Hold off
  double swpreset;      // sweep preset value
  double fstchan;		// acquisition delay
  double timepreset;    // time preset
} BOARDSETTING;

typedef struct {
  int nDevices;          // Number of channels = number of modules * (6 + 8) 
  int nDisplays;         // Number of histograms = nDevices + Positions + Maps
  int nSystems;          // Number of independent systems = 1
  int bRemote;           // 1 if server controlled by MPANT
  unsigned int sys;      // System definition word:
                           // bit0=0, bit1=0: dev#0 in system 1
                           // bit0=1, bit1=0: dev#0 in system 2
                           // bit0=0, bit1=1: dev#0 in system 3
                           // bit0=1, bit1=1: dev#0 in system 4
                           // bit2..bit6: 
                           // bit6=1, bit7=1: dev#3 in system 4 
						   // bit 31: any preset stops all
  int sys0[56];           // (reserved:) System definition words for CHN1..18:
                           // bit 0 CHN active
                           // bit 1 =1 CHN coinc, =0 single
                           // bit 2..4 CHN in system1..7
  int sys1[56];           // (reserved:) CHN in System
} ACQDEF;

typedef struct {
  unsigned int adcnum;     // Number of active ADC's (=0) 
  unsigned int tofnum;     // Number of active TOF channels 
  unsigned int ntofs0;	   // Number of TOF inputs 
  unsigned int modules;    // Number of MCS6A modules
  unsigned int nadcs;      // Number of ADCs (=0)

  int sys0[56];				// System definition words for ADCs (1..24):
							// see active definition in ADCSETTING
  int sys1[56];				// ADC in System (=1)
  int adcs[8];				// Number of ADCs per module (0)
  int tofs[8];				// Number of TOF inputs per module
  int speed[8];				// TOF speed in module: 3=100ps, 2=200ps, 1=400ps, 0=800ps
} COINCDEF;

typedef struct{
  unsigned int HUGE *s0;          // pointer to spectrum
  unsigned int *region;       // pointer to regions
  unsigned char *comment0;     // pointer to strings
  double *cnt;                 // pointer to counters
  HANDLE hs0;
  HANDLE hrg;
  HANDLE hcm;
  HANDLE hct;
} ACQDATA;

typedef struct {
  int nDevices;          // Number of channels = number of modules * 6
  int nDisplays;         // Number of histograms = nDevices + Positions + Maps
  int nSystems;          // Number of independent systems = 1
  int bRemote;           // 1 if server controlled by MPANT
  unsigned int sys;      // System definition word:
                           // bit0=0, bit1=0: dev#0 in system 1
                           // bit0=1, bit1=0: dev#0 in system 2
                           // bit0=0, bit1=1: dev#0 in system 3
                           // bit0=1, bit1=1: dev#0 in system 4
                           // bit2..bit6: 
                           // bit6=1, bit7=1: dev#3 in system 4 
} LVACQDEF;

typedef struct {
  int savedata;         // bit 0: auto save after stop
						 // bit 1: write listfile
						 // bit 2: listfile only, no evaluation
  int autoinc;          // 1 if auto increment filename
  int fmt;              // format type (seperate spectra): 
						 // 0 == ASCII, 1 == binary,
                         // 2 == CSV
  int mpafmt;           // format used in mpa datafiles 
  int sephead;          // seperate Header 
  int smpts;
  int caluse;
} LVDATSETTING;

typedef struct {
  int use;              // 1 if Replay Mode ON
  int modified;         // Bit 0: 1 if different settings are used
						 // (Bit 1: Write ASCII, reserved)
  int limit;            // 0: all, 
                        // 1: limited sweep range
  int speed;			// replay speed in units of 100 kB / sec
  double startsfrom;    // first start#
  double startsto;      // last start#
  double startspreset;  // last start - first start
} LVREPLAYSETTING;

typedef struct {
  unsigned int adcnum;     // Number of active ADC's (=0) 
  unsigned int tofnum;     // Number of active TOF channels 
  unsigned int ntofs0;	   // Number of TOF inputs 
  unsigned int modules;    // Number of MCS6A modules
  unsigned int nadcs;      // Number of ADCs (=0)

} LVCOINCDEF;


