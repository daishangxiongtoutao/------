using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace MCS8Demo1
{
    static class Constants
    {
        public const int ST_RUNTIME =   0;
        public const int ST_OFLS =      1;
        public const int ST_TOTALSUM =  2;
        public const int ST_ROISUM =    3;
        public const int ST_ROIRATE =   4;
        public const int ST_SWEEPS =    5;
        public const int ST_STARTS =    6;
        public const int ST_ZEROEVTS =  7;
    }
    // MCS Channel Status
    public struct ACQSETTING
    {
            public int range;            // spectrum length
            public int cftfak;           // LOWORD: 256 * cft factor (t_after_peak / t_to_peak)
						                 // HIWORD: max pulse width for CFT 
            public int roimin;           // lower ROI limit
            public int roimax;           // upper limit: roimin <= channel < roimax
            public int nregions;         // number of regions
            public int caluse;           // bit0: 1 if calibration used, higher bits: formula
            public int calpoints;        // number of calibration points
            public int param;            // (reserved:) for MAP and POS: LOWORD=x, HIWORD=y 
            public int offset;           // (reserved:) zoomed MAPS: LOWORD: xoffset, HIWORD, yoffset
            public int xdim;			//  (reserved:) x resolution of maps
            public int bitshift;  	// LOWORD: Binwidth = 2 ^ (bitshift)
        						    // HIWORD: Threshold for Coinc
            public int active;      	// Spectrum definition words for CHN1..6:
                           // active & 0xF  ==0 not used 
                           //               ==1 single
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
            public double eventpreset;      // ROI preset value
            public double dummy1;        // (Livetime preset)
            public double dummy2;		// (Realtime preset)
            public double dummy3;        // 
    }

    unsafe public struct DATSETTING
    {
              public int savedata;         // bit 0: auto save after stop
					   // bit 1: write listfile
					   // bit 2: listfile only, no evaluation
					   // bit 5: drop zero events
              public int autoinc;          // 1 if auto increment filename
              public int fmt;              // format type (seperate spectra): 
					   // 0 == ASCII, 1 == binary,
                         		   // 2 == GANAAS, 3 == EMSA, 4 == CSV
              public int mpafmt;           // format used in mpa datafiles 
              public int sephead;          // seperate Header 
              public int smpts;
              public int caluse;
              unsafe public fixed byte filename[256];
              unsafe public fixed byte specfile[256];
              unsafe public fixed byte command[256];
    }

    public struct BOARDSETTING
    {
                public int sweepmode;         // sweepmode & 0xF: 0 = normal, 
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
						// bit 14: ch6 ref (diff.mode)
                        // bit 16..bit 22 ~(input channel enable) 
						// bit 24: require data lost bit in data
						// bit 25: don't allow 6 byte datalength
						// bit 27: Folded
						// bit 28: Interleaved
                public int prena;          // bit 0: realtime preset enabled
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
                public int cycles;          // for sequential mode
                public int sequences;       // for sequential mode
                public int syncout;			// LOWORD: sync out; bit 0..5 NIM syncout, bit 8..13 TTL syncout
						// bit7: NIM syncout_invert, bit15: TTL syncout_invert
						// 0="0", 1=5 MHz, 2=50 MHz, 3=100 MHz, 4=97.656 MHz,
						// 5=195.625 MHz, 6= 195 MHz (int ref), 7=Start, 8=Ch1, 9=Ch2, 10=Ch3,
						// 11=Ch4, 12=Ch5, 13=Ch6, 14=Ch7, 15=GO, 16=Start_of_sweep, 17=Armed,
						// 18=SWEEP_ON, 19=WINDOW, 20=HOLD_OFF, 21=EOS_DEADTIME
						// 22=TIME[0],...,51=TIME[29], 52...63=SWEEP[0]..SWEEP[11]
                        //
                public int digio;           // LOWORD: Use of Dig I/O, GO Line:
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
                public int digval;			// digval=0..255 value for samplechanger 
                public int dac0;         // DAC0 value (START) 
						//  bit 16: Start with rising edge
                public int dac1;           //  DAC1 value (STOP 1) 
                public int dac2;           //  DAC2 value (STOP 2)
                public int dac3;           //  DAC3 value (STOP 3)
                public int dac4;				//  DAC4 value (STOP 4) 
                public int dac5;				//  DAC5 value (STOP 5)
						// bit (14,15) of each word: 0=falling, 1=rising, 2=both, 3=both+CFT 
						// bit 17 of each: pulse width mode under threshold
                public int fdac;	     // dummy
                public int tagbits;      // number of tagbits
                public int extclk;		// use external clock
                public int periods;			// number of periods in folded mode, sweeplength = range * periods
                public int serno;		// serial number
                public int ddruse;              // bit0: DDR_USE, bit1: DDR_2GB
						// bits[2:3]: usb_usage
						// bits[4:5]: wdlen
                public int active;          // module in system
                public double holdafter;	    // Hold off
                public double swpreset;      // sweep preset value
                public double fstchan;		// acquisition delay
                public double timepreset;    // time preset
    }

    public struct LVCOINCDEF
    {
        public int adcnum;     // Number of active ADC's 
        public int tofnum;     // Number of active MCS/Scope channels 
        public int ntofs0;	   // Number of TOF inputs
        public int modules;    // Number of MCS8 modules
        public int nadcs;      // Number of ADCs
    }

    unsafe public struct ACQSTATUS
    {
        public int started;        	// aquisition status: 1 if running, 0 else
        public int maxval;
        unsafe public fixed double cnt[8];  		// see ST_.. defines above
    }
    class Program
    {
        // imports from DMCS8.DLL
        [DllImport("dmcs8.dll", EntryPoint = "RunCmd")]
        extern static void RunCmd(int nDisplay, string Command);
        //		private static extern void RunCmd(int nDisplay, StringBuilder Command);
        [DllImport("dmcs8.dll", EntryPoint = "GetStatus")]
        extern static int GetStatus(int nDevice);
        [DllImport("dmcs8.dll", EntryPoint = "GetStatusData")]
        extern static int GetStatusData(ref ACQSTATUS Status, int nDevice);
        [DllImport("dmcs8.dll", EntryPoint = "GetSettingData")]
        extern static int GetSettingData(ref ACQSETTING MSetting, int nDisplay);
        [DllImport("dmcs8.dll", EntryPoint = "LVGetCnt")]
        extern static int LVGetCnt(ref double cntp, int nDisplay);
        [DllImport("dmcs8.dll", EntryPoint = "LVGetRoi")]
        extern static int LVGetRoi(ref int roip, int nDisplay);
        [DllImport("dmcs8.dll", EntryPoint = "LVGetDat")]
        extern static int LVGetDat(ref int datp, int nDisplay);
        [DllImport("dmcs8.dll", EntryPoint = "LVGetCDefData")]
        extern static int LVGetCDefData(ref LVCOINCDEF LCDef);
        [DllImport("dmcs8.dll", EntryPoint = "GetMCSSetting")]
        extern static int GetMCSSetting(ref BOARDSETTING BoardSetting, int nDevice);
        [DllImport("dmcs8.dll", EntryPoint = "GetDatSetting")]
        extern static int GetDatSetting(ref DATSETTING DatSetting);
        [DllImport("dmcs8.dll", EntryPoint = "GetBlock")]
        extern static void GetBlock(ref int datp, int from, int to, int step, int nDisplay); 

        static void Main(string[] args)
        {
            int nDev = 0;
            ACQSETTING acq;
            ACQSTATUS Status;
            String command;
            LVCOINCDEF LCDef;
            BOARDSETTING BoardSetting;
            LCDef = new LVCOINCDEF();
            BoardSetting = new BOARDSETTING ();

            acq = new ACQSETTING();
            Status = new ACQSTATUS();
            GetStatus(0);
            GetStatusData(ref Status, 0);
            LVGetCDefData(ref LCDef);
            GetMCSSetting(ref BoardSetting, 0);
            PrintMPAStatus(ref Status);
            Console.WriteLine();
                GetSettingData(ref acq, nDev);
                PrintSetting(ref acq);
            help();
            while (true)
            {
                command = Console.ReadLine();
                if (run(command, ref nDev) == 1) break;
            }
        }

        unsafe static void PrintMPAStatus(ref ACQSTATUS Status)
        {
            fixed (ACQSTATUS* s = &Status) { 
                if ((s->started & 0x01) != 0) Console.WriteLine("ON"); 
	            else if ((s->started & 0x02) != 0) Console.WriteLine("READ OUT"); 
	            else Console.WriteLine("OFF");
                Console.Write("runtime= ");
                Console.WriteLine(s->cnt[Constants.ST_RUNTIME]);
                Console.Write("sweeps= ");
                Console.WriteLine(s->cnt[Constants.ST_SWEEPS]);
                Console.Write("starts= ");
                Console.WriteLine(s->cnt[Constants.ST_STARTS]);
	        }
        }

        unsafe static void PrintStatus(ref ACQSTATUS Status)
        {
           fixed (ACQSTATUS* s = &Status) { 
                    Console.Write("total= ");
                    Console.WriteLine(s->cnt[Constants.ST_TOTALSUM]);
                    Console.Write("roi= ");
                    Console.WriteLine(s->cnt[Constants.ST_ROISUM]);
                    Console.Write("rate= ");
                    Console.WriteLine(s->cnt[Constants.ST_ROIRATE]);
                    Console.Write("ofls= ");
                    Console.WriteLine(s->cnt[Constants.ST_OFLS]);
	        }
        }

        unsafe static void PrintDatSetting(ref DATSETTING Set)
        {
            fixed (DATSETTING* d = &Set)
            {
                byte[] f = new byte[255];
                string s;
                int i;
                for (i=0; i<255; i++)
                    f[i] = d->filename[i];
                s = Encoding.UTF8.GetString(f, 0, 255);
                Console.Write("savedata= ");
                Console.WriteLine(d->savedata);
                Console.Write("autoinc= ");
                Console.WriteLine(d->autoinc);
                Console.Write("fmt= ");
                Console.WriteLine(d->fmt);
                Console.Write("mpafmt= ");
                Console.WriteLine(d->mpafmt);
                Console.Write("sephead= ");
                Console.WriteLine(d->sephead);
                Console.Write("filename= ");
                Console.WriteLine(s);
            }
        }

        static void PrintMCSSetting(ref BOARDSETTING Set)
        {
            Console.Write("sweepmode= ");
            Console.WriteLine(Set.sweepmode);
            Console.Write("prena= ");
            Console.WriteLine(Set.prena);
            Console.Write("cycles= ");
            Console.WriteLine(Set.cycles);
            Console.Write("sequences= ");
            Console.WriteLine(Set.sequences);
            Console.Write("digio= ");
            Console.WriteLine(Set.digio);
            Console.Write("digval= ");
            Console.WriteLine(Set.digval);
            Console.Write("dac0= ");
            Console.WriteLine(Set.dac0);
            Console.Write("dac1= ");
            Console.WriteLine(Set.dac1);
            Console.Write("dac2= ");
            Console.WriteLine(Set.dac2);
            Console.Write("dac3= ");
            Console.WriteLine(Set.dac3);
            Console.Write("dac4= ");
            Console.WriteLine(Set.dac4);
            Console.Write("dac5= ");
            Console.WriteLine(Set.dac5);
            Console.Write("serno= ");
            Console.WriteLine(Set.serno);
            Console.Write("ddruse= ");
            Console.WriteLine(Set.ddruse);
            Console.Write("active= ");
            Console.WriteLine(Set.active);
            Console.Write("holdafter= ");
            Console.WriteLine(Set.holdafter);
            Console.Write("swpreset= ");
            Console.WriteLine(Set.swpreset);
            Console.Write("fstchan= ");
            Console.WriteLine(Set.fstchan);
            Console.Write("timepreset= ");
            Console.WriteLine(Set.timepreset);
        }

        static void PrintSetting(ref ACQSETTING acq)
        {
            Console.Write("range= ");
            Console.WriteLine(acq.range);
            Console.Write("cftfak= ");
            Console.WriteLine(acq.cftfak);
            Console.Write("roimin= ");
            Console.WriteLine(acq.roimin);
            Console.Write("roimax= ");
            Console.WriteLine(acq.roimax);
            Console.Write("nregions= ");
            Console.WriteLine(acq.nregions);
            Console.Write("caluse= ");
            Console.WriteLine(acq.caluse);
            Console.Write("calpoints= ");
            Console.WriteLine(acq.calpoints);
            Console.Write("active= ");
            Console.WriteLine(acq.active);
            Console.Write("roipreset= ");
            Console.WriteLine(acq.eventpreset);
        }

         static int run(string command, ref int nDev)
        {

            int[] Spec = new int[30];
            if (command == "H") help();
            else if (command == "Q")
            {
                return 1;
            }
            else if (command == "T")
            {
                ACQSETTING MSetting;
                Console.Write("CHN ");
                Console.WriteLine(nDev);
                MSetting = new ACQSETTING();
                GetSettingData(ref MSetting, nDev);
                PrintSetting(ref MSetting);
            }
            else if (command == "S")
            {
                ACQSTATUS Status;
                Status = new ACQSTATUS();
                GetStatusData(ref Status, nDev);
                PrintStatus(ref Status);
            }
            else if (command == "D")
            {
                ACQSETTING MSetting;
                MSetting = new ACQSETTING();
                GetSettingData(ref MSetting, nDev);
//                for (i=0; i<30; i++)
//                    GetBlock(ref Spec[i], i, i+1, 1, nDev);
                GetBlock(ref Spec[0], 0, 30, 1, nDev);
                PrintDat(MSetting.range, ref Spec);
            }
            else if (command == "F")
            {
                DATSETTING DSetting;
                DSetting = new DATSETTING();
                GetDatSetting(ref DSetting);
                PrintDatSetting(ref DSetting);
            }
            else
            {
                RunCmd(0, command);
            }
            return 0;
        }
        
        static void help()
        {
            Console.WriteLine("Commands:");
            Console.WriteLine("Q	Quit");
            Console.WriteLine("H	Help");
            Console.WriteLine("S    Status");
            Console.WriteLine("T    Setting");
            Console.WriteLine("D    Data");
            Console.WriteLine("F    Datsetting");
            Console.WriteLine("(... more see command language in MPANT help)");
            Console.WriteLine();
        }

        static void PrintDat(int range, ref int[] datp)
        {
            int i;
            Console.Write("first 30 of ");
            Console.Write(range);
            Console.WriteLine(" datapoints:");
            for (i = 0; i < 30; i++)
                Console.WriteLine(datp[i]);
        }
    }
}
