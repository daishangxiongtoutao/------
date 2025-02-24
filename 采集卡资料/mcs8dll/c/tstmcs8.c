// --------------------------------------------------------------------------
// TSTMCS8.C : DMCS8.DLL Software driver C example
// --------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#undef DLL
#include "dmcs8.h"


HANDLE          hDLL = 0;

IMPAGETSETTING  lpSet=NULL;
IMPANEWSTATUS   lpNewStat=NULL;
IMPAGETSTATUS   lpStat=NULL;
IMPARUNCMD      lpRun=NULL;
IMPAGETCNT      lpCnt=NULL;
IMPAGETROI      lpRoi=NULL;
IMPAGETDAT      lpDat=NULL;
IMPAGETSTR      lpStr=NULL;
IMPASERVEXEC    lpServ=NULL;
IMPAGETDATSET   lpGetDatSet=NULL;
IMPAGETMCSSET   lpGetMCSSet=NULL;
IMPADIGINOUT    lpDigInOut=NULL;
IMPASTART		lpStart=NULL;
IMPAHALT		lpHalt=NULL;
IMPACONTINUE    lpContinue=NULL;
IMPAERASE		lpErase=NULL;


ACQSETTING     Setting={0};
ACQDATA        Data={0};
ACQDEF         Def={0};
ACQSTATUS      Status={0};
DATSETTING     DatSetting={0};
BOARDSETTING   MCSSetting={0};

int nDev=0;

void help()
{
	printf("Commands:\n");
	printf("Q		Quit\n");
	printf("?		Help\n");
	printf("S       Show Status\n");
	printf("H		Halt\n");
	printf("T       Show Setting\n");
	printf("B       Show BoardSetting\n");
	printf("CHN=x   Switch to CHN #x \n");
	printf("rdig    Read Dig. I/O port\n");
	  printf("w5 aabb		Write aabb into Reg.5\n");
	  printf("r5		Read Reg 5\n");
	  printf("rx0		Read Reg 0..f\n");
	  printf("rx7		Read Reg 70..7f\n");
	  printf("reep0		Read 64 byte from eeprom at address 0\n");
	  printf("sleep=10	Sleep 10 msec\n");
	  printf("run filename\n");
    printf("(... more see command language in MPANT help)\n");
    printf("\n");
}

void PrintMpaStatus(ACQSTATUS *Stat)
{
  if(Stat->started == 1) printf("ON\n"); 
  else if(Stat->started & 0x02) printf("READ OUT\n");
  else printf("OFF\n");
  printf("runtime=   %.3lf\n", Stat->cnt[ST_RUNTIME]);
  printf("sweeps=   %lf\n", Stat->cnt[ST_SWEEPS]);
  printf("starts=   %lf\n\n", Stat->cnt[ST_STARTS]);
}

void PrintStatus(ACQSTATUS *Stat)
{
  printf("totalsum=   %.0lf\n", Stat->cnt[ST_TOTALSUM]);
  printf("roisum=     %.0lf\n", Stat->cnt[ST_ROISUM]);
  printf("rate=     %.2lf\n", Stat->cnt[ST_ROIRATE]);
  printf("ofls=     %.2lf\n\n", Stat->cnt[ST_OFLS]);
}

void PrintDatSetting(DATSETTING *Set)
{
  printf("savedata= %d\n", Set->savedata);
  printf("autoinc=  %d\n", Set->autoinc);
  printf("fmt=      %d\n", Set->fmt);
  printf("mpafmt=   %d\n", Set->mpafmt);
  printf("sephead=  %d\n", Set->sephead);
  printf("filename= %s\n\n", Set->filename);
}

void PrintMCSSetting(BOARDSETTING *Set)
{
  printf("sweepmode=  0x%x\n", Set->sweepmode);
  printf("prena=      0x%x\n", Set->prena);
  printf("cycles=     %d\n", Set->cycles);
  printf("sequences=  %d\n", Set->sequences);
  printf("syncout=    0x%x\n", Set->syncout);
  printf("digio=      0x%x\n", Set->digio);
  printf("digval=     %d\n", Set->digval);
  printf("dac0=       0x%x\n", Set->dac0);
  printf("dac1=       0x%x\n", Set->dac1);
  printf("dac2=       0x%x\n", Set->dac2);
  printf("dac3=       0x%x\n", Set->dac3);
  printf("dac4=       0x%x\n", Set->dac4);
  printf("dac5=       0x%x\n", Set->dac5);
//  printf("fdac=       0x%x\n", Set->fdac);
  printf("tagbits=    %d\n", Set->tagbits);
  printf("extclk=     %d\n", Set->extclk);
  printf("periods=    %d\n", Set->periods);
  printf("serno=      %d\n", Set->serno);
  printf("ddruse=     0x%x\n", Set->ddruse);
  printf("active=     %d\n", Set->active);
  printf("holdafter=  %lg\n", Set->holdafter);
  printf("swpreset=   %lg\n", Set->swpreset);
  printf("fstchan=    %lg\n", Set->fstchan);
  printf("timepreset= %lg\n\n", Set->timepreset);
}

void PrintSetting(ACQSETTING *Set)
{
  printf("range=     %ld\n", Set->range);
  printf("cftfak=    0x%x\n", Set->cftfak);
  printf("roimin=    %ld\n", Set->roimin);
  printf("roimax=    %ld\n", Set->roimax);
  printf("nregions=  %d\n", Set->nregions);
  printf("caluse=    %d\n", Set->caluse);
  printf("calpoints= %d\n", Set->calpoints);
  printf("param=     0x%lx\n", Set->param);
  printf("offset=    0x%lx\n", Set->offset);
  printf("xdim=      %d\n", Set->xdim);
  printf("bitshift=  %d\n", Set->bitshift);
  printf("active=    0x%x\n", Set->active);
  printf("roipreset= %lg\n\n", Set->eventpreset);
}


int run(char *command)
{
	int err;
	if (!stricmp(command, "?"))           help();
	else if (!stricmp(command, "rdig")) {		// read Dig I/O port
	  if (lpDigInOut) {
	    err = (*lpDigInOut)(0xff,0);
	    printf("%x\n", err);
	  }
	}
	else if (!strnicmp(command, "wdig=", 5)) {	// write Dig I/O port (open drain)
	  if (lpDigInOut) {
		unsigned int val;
		sscanf(command+5, "%x", &val);
		val &= 0xff;
	    err = (*lpDigInOut)(val,0xff);
	    printf("%x\n", err);
	  }
	}
	else if (!strnicmp(command, "pdig=", 5)) {	// write Dig I/O port (push-pull)
	  if (lpDigInOut) {
		unsigned int val;
		sscanf(command+5, "%x", &val);
	    err = (*lpDigInOut)(val,0xff);
	    printf("%x\n", err);
	  }
	}
	else if (!stricmp(command,"Q"))       return 1;
	else if (!stricmp(command,"S")) {
      err = (*lpStat)(&Status, nDev);
	  if (nDev) PrintStatus(&Status);
	  else PrintMpaStatus(&Status);
	  return 0;
	}
	else if (!stricmp(command,"T")) {
	  	// spectra settings
        err = (*lpSet)(&Setting, nDev);
		printf("CHN %d:\n", nDev);
		PrintSetting(&Setting);
	  
	  if (nDev==0) {        // MPA settings
        err = (*lpGetMCSSet)(&MCSSetting, 0);
		PrintMCSSetting(&MCSSetting);
		            // DATSettings
        err = (*lpGetDatSet)(&DatSetting);
		PrintDatSetting(&DatSetting);
	  }
	  
	  return 0;
	}

	else if (!stricmp(command,"H")) {
      (*lpHalt)(0);
	  return 0;
	}
	else if(!strnicmp(command, "CHN=", 4)) {
	  sscanf(command+4, "%d", &nDev);
	  (*lpRun)(0, command);
	}
	else if (!stricmp(command,"MPA")) {
	  nDev=0;
	  (*lpRun)(0, command);
	}
	else {
		(*lpRun)(0, command);
		printf("%s\n", command);
	}
	return 0;
}

int readstr(char *buff, int buflen)
{
  int i=0,ic;

  while ((ic=getchar()) != 10) {
    if (ic == EOF) {
      buff[i]='\0';
      return 1;
    }
    if (ic == 13) ic=0;
    buff[i]=(char)ic;
    i++;
    if (i==buflen-1) break;
  }
  buff[i]='\0';
  return 0;
}

//int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow)
void main(int argc, char *argv[])  
{
  long Errset=0;
  char command[4096]={0};

  hDLL = LoadLibrary("DMCS8.DLL");
  if(hDLL){
      lpSet=(IMPAGETSETTING)GetProcAddress(hDLL,"GetSettingData");
	  lpNewStat=(IMPANEWSTATUS)GetProcAddress(hDLL,"GetStatus");
	  lpStat=(IMPAGETSTATUS)GetProcAddress(hDLL,"GetStatusData");
	  lpRun=(IMPARUNCMD)GetProcAddress(hDLL,"RunCmd");
	  lpCnt=(IMPAGETCNT)GetProcAddress(hDLL,"LVGetCnt");
	  lpRoi=(IMPAGETROI)GetProcAddress(hDLL,"LVGetRoi");
	  lpDat=(IMPAGETDAT)GetProcAddress(hDLL,"LVGetDat");
	  lpStr=(IMPAGETSTR)GetProcAddress(hDLL,"LVGetStr");
	  lpServ=(IMPASERVEXEC)GetProcAddress(hDLL,"ServExec");
	  lpGetDatSet=(IMPAGETDATSET)GetProcAddress(hDLL,"GetDatSetting");
	  lpGetMCSSet=(IMPAGETMCSSET)GetProcAddress(hDLL,"GetMCSSetting");
	  lpStart=(IMPASTART)GetProcAddress(hDLL,"Start");
	  lpHalt=(IMPAHALT)GetProcAddress(hDLL,"Halt");
	  lpContinue=(IMPACONTINUE)GetProcAddress(hDLL,"Continue");
	  lpErase=(IMPAERASE)GetProcAddress(hDLL,"Erase");
	  lpDigInOut=(IMPADIGINOUT)GetProcAddress(hDLL,"DigInOut");
  }
  else return;

  // Initialize parameters

  Errset = (*lpNewStat)(0);
  Errset = (*lpStat)(&Status, 0);
  PrintMpaStatus(&Status);

  help();

  while(TRUE)
	{
		readstr(command, 80);
		if (run(command)) break;
	}

  FreeLibrary(hDLL);

  return;
}
