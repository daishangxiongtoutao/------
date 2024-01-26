/***************************************************************************
  MODULE:   DMCS8.C
  PURPOSE:  DLL to communicate with MCS8 Server
****************************************************************************/

#include "windows.h"
#include <string.h>
#include <stdio.h>
#define DLL
#include "dmcs8.h"

#pragma data_seg("dmcs8sh")

ACQSTATUS DLLStatus[MAXDSP] = {0};
EXTACQSETTING DLLSetting[MAXDSP] = {0};
ACQDEF DLLDef = {0};
COINCDEF DLLCDef = {0};
BOARDSETTING DLLmc[MAXDEV] = {0};
DATSETTING DLLdat = {0};
REPLAYSETTING DLLRepl = {0};

BOOL    bRemote=0;
BOOL    bStatus[MAXDSP]={0};
BOOL    bSetting[MAXDSP]={0};
BOOL    bDef=FALSE;
BOOL    bCDef=FALSE;
BOOL    bmc[MAXDEV]={0};
BOOL    bDat=FALSE;
BOOL    bRepl=FALSE;

int		scdevices=0;
SCSETTING scaler[MAXSCDEV] = {0};
SCINFO    scinfo[MAXSCDEV] = {0};
double scstartclock[MAXSCDEV]={0.};
double scrtime0[MAXSCDEV]={0.};
double scvaloff[MAXSCDEV*NUMSCALER]={0.};
HWND    hwndServer=0;
HWND    hwndClient=0;
HWND    hwndMPANT=0;
UINT    MM_NEARCONTROL=0;
UINT    MM_GETVAL=0;


#pragma data_seg()

//HWND    hwndMCDWIN=0;


BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{
    return 1;
        UNREFERENCED_PARAMETER(hInst);
        UNREFERENCED_PARAMETER(ul_reason_being_called);
        UNREFERENCED_PARAMETER(lpReserved);
}

VOID APIENTRY StoreDefData(ACQDEF  *Def)
{
  int i;
  if(Def == NULL) {
    bDef = FALSE;
    for (i=0; i<MAXDSP; i++) {
      bSetting[i] = FALSE;
      bStatus[i] = FALSE;
    }
  }
  else{
    _fmemcpy((LPSTR  *)&DLLDef,(LPSTR  *)Def,sizeof(ACQDEF));
    bDef = TRUE;
  }
}

int APIENTRY GetDefData(ACQDEF  *Def)
{
  if (bDef) {
    DLLDef.bRemote = bRemote;
    _fmemcpy((LPSTR  *)Def,(LPSTR  *)&DLLDef,sizeof(ACQDEF));
  }
  return bDef;
}

VOID APIENTRY StoreCDefData(COINCDEF  *Def)
{
  if(Def == NULL) {
    bCDef = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLCDef,(LPSTR  *)Def,sizeof(COINCDEF));
    bCDef = TRUE;
  }
}

int APIENTRY GetCDefData(COINCDEF  *Def)
{
  if (bCDef) {
    _fmemcpy((LPSTR  *)Def,(LPSTR  *)&DLLCDef,sizeof(COINCDEF));
  }
  return bCDef;
}

VOID APIENTRY StoreDatSetting(DATSETTING *Defdat)
{
  if(Defdat == NULL) {
    bDat = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLdat,(LPSTR  *)Defdat,sizeof(DATSETTING));
    bDat = TRUE;
  }
}

int APIENTRY GetDatSetting(DATSETTING *Defdat)
{
  if (bDat) {
    _fmemcpy((LPSTR  *)Defdat,(LPSTR  *)&DLLdat,sizeof(DATSETTING));
  }
  return bDat;
}

VOID APIENTRY StoreReplaySetting(REPLAYSETTING *Repldat)
{
  if(Repldat == NULL) {
    bRepl = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLRepl,(LPSTR  *)Repldat,sizeof(REPLAYSETTING));
    bRepl = TRUE;
  }
}

int APIENTRY GetReplaySetting(REPLAYSETTING *Repldat)
{
  if (bRepl) {
    _fmemcpy((LPSTR  *)Repldat,(LPSTR  *)&DLLRepl,sizeof(REPLAYSETTING));
  }
  return bRepl;
}

VOID APIENTRY StoreMCSSetting(BOARDSETTING *Defmc, int ndev)
{
  if (ndev < 0 || ndev >= MAXDEV) return;
  if(Defmc == NULL) {
    bmc[ndev] = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLmc[ndev],(LPSTR  *)Defmc,sizeof(BOARDSETTING));
    bmc[ndev] = TRUE;
  }
}

int APIENTRY GetMCSSetting(BOARDSETTING *Defmc, int ndev)
{
  if (ndev < 0 || ndev >= MAXDEV) return 0;
  if (bmc[ndev]) {
    _fmemcpy((LPSTR  *)Defmc,(LPSTR  *)&DLLmc[ndev],sizeof(BOARDSETTING));
  }
  return bmc[ndev];
}

VOID APIENTRY StoreSettingData(ACQSETTING  *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Setting == NULL) {
    bSetting[nDisplay] = FALSE;
    bStatus[nDisplay] = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLSetting[nDisplay],
           (LPSTR  *)Setting,sizeof(ACQSETTING));
    bSetting[nDisplay] = TRUE;
    if(Setting->range == 0L) {
      bSetting[nDisplay] = FALSE;
      bStatus[nDisplay] = FALSE;
    }
  }
}

VOID APIENTRY StoreExtSettingData(EXTACQSETTING  *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Setting == NULL) {
    bSetting[nDisplay] = FALSE;
    bStatus[nDisplay] = FALSE;
  }
  else{
    _fmemcpy((LPSTR  *)&DLLSetting[nDisplay],
           (LPSTR  *)Setting,sizeof(EXTACQSETTING));
    bSetting[nDisplay] = TRUE;
    if(Setting->range == 0L) {
      bSetting[nDisplay] = FALSE;
      bStatus[nDisplay] = FALSE;
    }
  }
}

int APIENTRY GetSettingData(ACQSETTING  *Setting, int nDisplay)
{
//DebugBreak();
	if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR  *)Setting,
           (LPSTR  *)&DLLSetting[nDisplay],sizeof(ACQSETTING));
  }
  return bSetting[nDisplay];
}

int APIENTRY GetExtSettingData(EXTACQSETTING  *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR  *)Setting,
           (LPSTR  *)&DLLSetting[nDisplay],sizeof(EXTACQSETTING));
  }
  return bSetting[nDisplay];
}

VOID APIENTRY StoreData(ACQDATA  *Data, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Data == NULL) {
    bSetting[nDisplay] = FALSE;
    bStatus[nDisplay] = FALSE;
  }
#ifdef WINDOWS31
  else
    _fmemcpy((LPSTR  *)&DLLData[nDisplay],(LPSTR  *)Data,sizeof(ACQDATA));
#endif
}

int APIENTRY GetData(ACQDATA  *Data, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR  *)Data,(LPSTR  *)&DLLData[nDisplay],sizeof(ACQDATA));
  }
#endif
  return bSetting[nDisplay];
}

int APIENTRY GetSpec(int i, int nDisplay)
{
#ifdef WINDOWS31
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay] && i < DLLSetting[nDisplay].range)
    return (DLLData[nDisplay].s0[i]);
  else return 0L;
#else
  char sz[40];
  HANDLE hs0;
  unsigned int *s0;
  unsigned int val;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (!bSetting[nDisplay]) return 0;
  if (i > DLLSetting[nDisplay].range) return 0;
  sprintf(sz,"MCS8_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 0;
  if (!(s0 = (unsigned int *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return 0;
  }
  val = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return val;
#endif
}

VOID APIENTRY GetBlock(int  *hist, int start, int end, int step,
  int nDisplay)
{
#ifdef WINDOWS31
  int i,j=0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (end > DLLSetting[nDisplay].range) end = DLLSetting[nDisplay].range;
  for (i=start; i<end; i+=step, j++)
    *(hist + j) = DLLData[nDisplay].s0[i];
#else
  int i,j=0;
  char sz[40];
  HANDLE hs0;
  unsigned int *s0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!bSetting[nDisplay]) return;
  if (end > DLLSetting[nDisplay].range) end = (int)DLLSetting[nDisplay].range;
  sprintf(sz,"MCS8_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return;
  if (!(s0 = (unsigned int *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return;
  }
  for (i=start; i<end; i+=step, j++)
    *(hist + j) = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return;
#endif
}

int APIENTRY LVGetDat(unsigned int  *datp, int nDisplay)
{
#ifdef WINDOWS31
  int i;
  if (bSetting[nDisplay]) {
    for (i=0; i<DLLSetting[nDisplay].range; i++)
      datp[i] = DLLData[nDisplay].s0[i];
    return 0;
  }
  else return 4;
#else
  int i;
  char sz[40];
  HANDLE hs0;
  unsigned int *s0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(s0 = (unsigned int *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return 4;
  }
  for (i=0; i<DLLSetting[nDisplay].range; i++)
      datp[i] = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return 0;
#endif
}

HANDLE hEXMDisplay=0;
unsigned int * EXMDisplay=NULL;

int APIENTRY GetDatInfo(int nDisplay, int *xmax, int *ymax)
{
//DebugBreak();
  if (nDisplay < 0 || nDisplay >= MAXDSP) return -1;
  if (!bSetting[nDisplay]) return -1;
  *xmax = DLLSetting[nDisplay].xdim;
  *ymax = DLLSetting[nDisplay].range;
  if (*xmax) *ymax /= *xmax;
  else {*xmax = *ymax; *ymax = 1;}
  return DLLSetting[nDisplay].range;
}

int APIENTRY GetDatPtr(int nDisplay, int *xmax, int *ymax, LPSTR *pt)
{
  char sz[40];
//DebugBreak();
  if (nDisplay < 0 || nDisplay >= MAXDSP) return -1;
  if (!bSetting[nDisplay]) return -1;
  *xmax = DLLSetting[nDisplay].xdim;
  *ymax = DLLSetting[nDisplay].range;
  if (*xmax) *ymax /= *xmax;
  else {*xmax = *ymax; *ymax = 1;}
  sprintf(sz,"MCS8_S0_%d",nDisplay);
  ReleaseDatPtr();
  if (!(hEXMDisplay = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 0;
  if (!(EXMDisplay = MapViewOfFile(hEXMDisplay,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hEXMDisplay);
    return 0;
  }
  *pt = (LPSTR) EXMDisplay;
  return (int)hEXMDisplay;
}

int APIENTRY ReleaseDatPtr()
{
  if(EXMDisplay)
	 UnmapViewOfFile(EXMDisplay);
  EXMDisplay = NULL;
  if(hEXMDisplay)
	 CloseHandle(hEXMDisplay);
  hEXMDisplay = 0;
  return 0;
}

int APIENTRY LVGetProiDat(int roiid, int x0, int y0, int xdim, int ydim, double *roisum, int *datp)
{
  int idisp, x,y, x1, y1, xmax, ymax, pos, ret;
  unsigned int *pt;
  idisp = (roiid / 100) - 1;
  ret = GetDatPtr(idisp, &xmax, &ymax, (LPSTR *)&pt);
  if (ret <= 0) return -1;
  *roisum = 0.;
  x1 = x0 + xdim;
  y1 = y0 + ydim;
  for (x = x0; x < x1; x++) {
    for (y = y0; y < y1; y++) {
	  pos = y * xmax + x;
      *datp *= *(pt + pos);
	  *roisum += *datp;
      datp++;
    }
  }
  ReleaseDatPtr();
  return 0;
}

int APIENTRY LVGetRoi(unsigned int  *roip, int nDisplay)
{
#ifdef WINDOWS31
  int i,n;
  n = 2 * DLLSetting[nDisplay].nregions;
  if (bSetting[nDisplay]) {
    for (i=0; i<n; i++)
      roip[i] = DLLData[nDisplay].region[i];
    return 0;
  }
  else return 4;
#else
  int i,n;
  char sz[40];
  HANDLE hrg;
  unsigned int *region;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_RG_%d",nDisplay);
  if (!(hrg = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(region = (unsigned int *)MapViewOfFile(hrg,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hrg);
    return 4;
  }
  n = 2 * DLLSetting[nDisplay].nregions;
  for (i=0; i<n; i++)
    roip[i] = region[i];
  UnmapViewOfFile(region);
  CloseHandle(hrg);
  return 0;
#endif
}

int APIENTRY LVGetOneRoi(int nDisplay, int roinum, int *x1, int *x2)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay] && (roinum > 0 && (roinum <= 128)) {
    *x1 = DLLData[nDisplay].region[2*(roinum-1)];
    *x2 = DLLData[nDisplay].region[2*(roinum-1)+1];
    return 0;
  }
  else return 4;
#else
  char sz[40];
  HANDLE hrg;
  unsigned int *region;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay] || (roinum < 1) || (roinum > 128)) return 4;
  sprintf(sz,"MCS8_RG_%d",nDisplay);
  if (!(hrg = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(region = (unsigned int *)MapViewOfFile(hrg,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hrg);
    return 4;
  }
  *x1 = region[2*(roinum-1)];
  *x2 = region[2*(roinum-1)+1];
  UnmapViewOfFile(region);
  CloseHandle(hrg);
  return 0;
#endif
}

int APIENTRY LVGetRoiRect(int nDisplay, int roinum, int *x0, int *y0, int *xdim, int *ydim, int *xmax)
{
  int ret, xrange, xch1, xch2, ymax, offset, x1, y1;
  unsigned int *pt;
  if (!GetDatPtr(nDisplay, &xrange, &ymax, (LPSTR *)&pt)) {
    return -6;
  } 			// Display not found

  xrange = DLLSetting[nDisplay].xdim;
  ymax = DLLSetting[nDisplay].range;
  if (xrange) ymax /= xrange;
  else {xrange = ymax; ymax = 1;}

  *xmax = xrange;
  ret = LVGetOneRoi(nDisplay, roinum, &xch1, &xch2);
  if (ret || !xrange) {
	  ReleaseDatPtr();
	  return -1;
  }
  *y0 = xch1 / xrange;
  offset = (*y0) * xrange;
  *x0 = xch1 - offset;
  y1 = xch2 / xrange;
  offset = y1 * xrange;
  x1 = xch2 - offset;
  *xdim = x1 - *x0;
  *ydim = y1 - *y0 + 1;
  return 0;
}

int APIENTRY LVGetRroiDat(int nDisplay, int roinum, int x0, int y0, int xdim, int ydim, int xmax,
				 double *RoiSum, int *datp, double *area)
{
  int x, x1, y, y1, pos, ymax;
  unsigned int *pt;
  unsigned int val;
  if (!GetDatPtr(nDisplay, &xmax, &ymax, (LPSTR *)&pt)) {
    return -6;
  } 			// Display not found

  x1 = x0 + xdim;
  y1 = y0 + ydim -1;
  *RoiSum = 0.;
  *area = 0.;
  for (x = x0; x < x1; x++) {
    for (y = y0; y <= y1; y++) {
      pos = y * xmax + x;
      val = *(pt + pos);
      *RoiSum += val;
      *datp = val;
      datp++;
      *area += 1.;
    }
  }
  ReleaseDatPtr();
  return 0;
}

int APIENTRY LVGetCnt(double  *cntp, int nDisplay)
{
#ifdef WINDOWS31
  int i;
  if (bSetting[nDisplay]) {
    for (i=0; i<MAXCNT; i++)
      cntp[i] = DLLData[nDisplay].cnt[i];
    return 0;
  }
  else return 4;
#else
  int i;
  char sz[40];
  HANDLE hct;
  double *cnt;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_CT_%d",nDisplay);
  if (!(hct = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(cnt = (double *)MapViewOfFile(hct,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hct);
    return 4;
  }
  for (i=0; i<MAXCNT; i++)
    cntp[i] = cnt[i];
  UnmapViewOfFile(cnt);
  CloseHandle(hct);
  return 0;
#endif
}

int APIENTRY LVGetOneCnt(double *cntp, int nDisplay, int cntnum)
                             // Get one Cnt number
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    *cntp = DLLData[nDisplay].cnt[cntnum];
    return 0;
  }
  else return 4;
#else
  char sz[40];
  HANDLE hct;
  double *cnt;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_CT_%d",nDisplay);
  if (!(hct = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(cnt = (double *)MapViewOfFile(hct,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hct);
    return 4;
  }
  *cntp = cnt[cntnum];
  UnmapViewOfFile(cnt);
  CloseHandle(hct);
  return 0;
#endif
}

int APIENTRY LVGetStr(char  *strp, int nDisplay)
{
#ifdef WINDOWS31
  int i;
  if (bSetting[nDisplay]) {
    for (i=0; i<1024; i++)
      strp[i] = DLLData[nDisplay].comment0[i];
    return 0;
  }
  else return 4;
#else
  int i;
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_CM_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hcm);
    return 4;
  }
  for (i=0; i<1024; i++)
    strp[i] = comment0[i];
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);
  return 0;
#endif
}

VOID APIENTRY StoreStatusData(ACQSTATUS  *Status, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Status == NULL)
    bStatus[nDisplay] = FALSE;
  else{
    _fmemcpy((LPSTR  *)&DLLStatus[nDisplay],
           (LPSTR  *)Status,sizeof(ACQSTATUS));
    bStatus[nDisplay] = TRUE;
  }
}

int APIENTRY GetStatusData(ACQSTATUS  *Status, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bStatus[nDisplay]) {
    _fmemcpy((LPSTR  *)Status,
           (LPSTR  *)&DLLStatus[nDisplay],sizeof(ACQSTATUS));
  }
  return bStatus[nDisplay];
}

VOID APIENTRY Start(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_START, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_START2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_START3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_START4, 0L);
     break;
  }
}

VOID APIENTRY Halt(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK4, 0L);
     break;
  }
}

VOID APIENTRY Continue(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE4, 0L);
     break;
  }
}

VOID APIENTRY SaveSetting()
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_SAVE, 0L);
}

VOID APIENTRY NewSetting(int nDev)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  //if (nDev>=0 && nDev<MAXDSP) bStatus[nDev] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_NEWSETTING, 0L);
}

VOID APIENTRY NewData()
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_NEWDATA, 0L);
}

int APIENTRY GetStatus(int nDev)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  if (bStatus[nDev]) {
      SendMessage(hwndServer, WM_COMMAND, ID_GETSTATUS, 0L);
  }
  return bStatus[nDev];
}

UINT APIENTRY ServExec(HWND ClientWnd)
{
  bRemote = 1;
  hwndClient = ClientWnd;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (hwndServer) {
//    ShowWindow(hwndServer, SW_MINIMIZE);
    return 32;
  }
  else
    return WinExec("mcs8.exe", SW_SHOW);
}

UINT APIENTRY ClientExec(HWND ServerWnd)
{
  if (ServerWnd) hwndServer = ServerWnd;
  return WinExec((LPSTR)"MPANT /device=MCS8", SW_SHOW);
}

VOID APIENTRY UnregisterClient()
{
  hwndClient = 0;
  bRemote = 0;
}

VOID APIENTRY DestroyClient()
{
  bRemote = 0;
  if (hwndClient) SendMessage(hwndClient, WM_CLOSE, 0, 0L);
  hwndClient = 0;
}

VOID APIENTRY Erase(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE, 0x10000L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE4, 0L);
     break;
  }
}

VOID APIENTRY SaveData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_SAVEFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY LoadData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_LOADFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY AddData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA$ Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SUMFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY SubData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SUBTRACT, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY Smooth(int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SMOOTH, 
		  MAKELPARAM((WORD)nDisplay, (WORD)0));
}

VOID APIENTRY HardwareDlg(int item)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server", NULL);
  switch (item) {
    case 0:
      PostMessage(hwndServer, WM_COMMAND, ID_HARDWDLG, 0L);
      break;
    case 1:
      PostMessage(hwndServer, WM_COMMAND, ID_DATADLG, 0L);
      break;
    case 2:
      PostMessage(hwndServer, WM_COMMAND, ID_COMBDLG, 0L);
      break;
    case 3:
      PostMessage(hwndServer, WM_COMMAND, ID_MAPLSTDLG, 0L);
      break;
    case 4:
      PostMessage(hwndServer, WM_COMMAND, ID_REPLDLG, 0L);
      break;
  }
}

VOID APIENTRY RunCmd(int nDisplay, LPSTR Cmd)
{
#ifdef WINDOWS31
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  if (Cmd != NULL) {
    _fstrcpy(&DLLData[0].comment0[800], Cmd);
  }
#else
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!bSetting[nDisplay]) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  sprintf(sz,"MCS8_CM_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_WRITE, FALSE, sz)))
	return;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_WRITE, 0, 0, 0))) {
    CloseHandle(hcm);
    return;
  }
  strcpy(&comment0[800], Cmd);
#endif
  SendMessage(hwndServer, MM_NEARCONTROL, (WPARAM)ID_RUNCMD, (LPARAM)(LPSTR)Cmd);
#ifndef WINDOWS31
  strcpy(Cmd, &comment0[1024]);
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);
#endif
}

int APIENTRY GetSVal(int DspID, int xval)
{
  int val=0;
  if (xval == -2) {
	  hwndMPANT = FIND_WINDOW("mpwframe-MCS8",NULL);
	  return (int)hwndMPANT;  
	  // should be called first to be sure that MPANT is started
  }
  if (!hwndMPANT) hwndMPANT = FIND_WINDOW("mpwframe-MCS8",NULL);
  if (!MM_GETVAL) MM_GETVAL = RegisterWindowMessage((LPSTR)"MCS8Getval");
  val = (int)SendMessage(hwndMPANT, MM_GETVAL, (WPARAM)DspID, (LPARAM)xval);
    // for xval == -1 returns Display size
  return val;
}

int APIENTRY BytearrayToShortarray(short *Shortarray, char *Bytearray, int length)
{
  int i;
  char c;
  for (i=0; i<length; i++) {
	  c = Bytearray[i];
	  Shortarray[i] = c;
	  if (!c) {
		  i++; break;
	  }
  }
  return i;
}

int APIENTRY LedBlink(int nDev)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  switch(nDev) {
  case 0:
	PostMessage(hwndServer, WM_COMMAND, ID_LEDBLINK0, 0); break;
  case 1:
	PostMessage(hwndServer, WM_COMMAND, ID_LEDBLINK1, 0); break;
  case 2:
	PostMessage(hwndServer, WM_COMMAND, ID_LEDBLINK2, 0); break;
  case 3:
	PostMessage(hwndServer, WM_COMMAND, ID_LEDBLINK3, 0); break;
  }
  return 0;
}

int APIENTRY DigInOut(int value, int enable)  // controls Dig I/0 ,
								  // returns digin
{
  int val=0;
  int lval;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  lval = ((int)value & 0xFF) | ((enable & 0xFF)<<8);
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_DIGINOUT, (LPARAM)lval);
  return val;
}

int APIENTRY LVGetCDefData(LVCOINCDEF *Def)
{
  if (bCDef) {
    _fmemcpy(Def,&DLLCDef,sizeof(LVCOINCDEF));
  }
  return bCDef;
}

int APIENTRY LVGetSpecLength(int nDisplay)
{
  if (bSetting[nDisplay]) 
	  return DLLSetting[nDisplay].range;
  else
	  return -1;
}

int APIENTRY LVGetDatSetting(LVDATSETTING *Defdat, LPSTR filename, LPSTR specfile, LPSTR command)
{
  int i;
  if (bDat) {
    _fmemcpy((LPSTR  *)Defdat,(LPSTR  *)&DLLdat,sizeof(LVDATSETTING));
	for (i=0; i<256; i++) {
	  filename[i] = DLLdat.filename[i];
	  specfile[i] = DLLdat.specfile[i];
	  command[i] = DLLdat.command[i];
	}
	return 0;
  }
  return -1;
}

int APIENTRY LVGetReplaySetting(LVREPLAYSETTING *Repldat, LPSTR filename)
{
  int i;
  if (bRepl) {
    _fmemcpy((LPSTR  *)Repldat,(LPSTR  *)&DLLRepl,sizeof(LVREPLAYSETTING));
	for (i=0; i<256; i++) {
	  filename[i] = DLLRepl.filename[i];
	}
	return 0;
  }
  return -1;
}

int APIENTRY LVGetDefData(LVACQDEF *Def)
{
  if (bDef) {
    DLLDef.bRemote = bRemote;
    _fmemcpy((LPSTR  *)Def,(LPSTR  *)&DLLDef,sizeof(LVACQDEF));
	return 0;
  }
  return -1;
}

int APIENTRY SetScSetting( SCSETTING *set, int nDev)
	  // Stores Scaler settings in DLL
{
	if ((nDev < 0) || (nDev >= MAXSCDEV)) return 0;
  _fmemcpy(&scaler[nDev], set, sizeof(SCSETTING));
  if (scdevices <= nDev) scdevices = nDev+1;
	return 1;
}

int APIENTRY GetScSetting(SCSETTING *set, int nDev)
	  // Get Scaler settings stored in DLL
{
	if (nDev == -1) return scdevices;
	if ((nDev < 0) || (nDev >= MAXSCDEV)) return 0;
  _fmemcpy(set, &scaler[nDev], sizeof(SCSETTING));
	return 1; 
}

int APIENTRY SetScInfo( SCINFO *sinfo, int nDev)
	  // Stores Scaler infosettings in DLL
{
	if ((nDev < 0) || (nDev >= MAXSCDEV)) return 0;
  _fmemcpy(&scinfo[nDev], sinfo, sizeof(SCINFO));
  if (scdevices <= nDev) scdevices = nDev+1;
	return 1;
}

int APIENTRY GetScInfo(SCINFO *set, int nDev)
	  // Get Scaler infosettings stored in DLL
{
	if (nDev == -1) return scdevices;
	if ((nDev < 0) || (nDev >= MAXSCDEV)) return 0;
	_fmemcpy(set, &scinfo[nDev], sizeof(SCINFO));
	return 1;
}

int APIENTRY GetRoiIndex(LPSTR roiname)
	// for named ROI's returns in LOWORD the spectra number, in HIWORD the roiid,
    // returns 0 if not found.
{
  int val=0;
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  strcpy(sz,"MCS8_CM_0");
  if (!(hcm = OpenFileMapping(FILE_MAP_WRITE, FALSE, sz)))
	return 0;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_WRITE, 0, 0, 0))) {
    CloseHandle(hcm);
    return 0;
  }
  strncpy(&comment0[800], roiname, 20);
  comment0[820] = '\0';

  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_GETROIINDEX, 0);
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);

  return val;
}

int APIENTRY DeleteRoi(DWORD roiindex)
    // deletes ROI
{
  int val=0;
  HWND hwndClient = FIND_WINDOW("mpwframe-MCS8",NULL);
  if (hwndClient) 
    return (int)SendMessage(hwndClient, WM_COMMAND, ID_DELETEROI, roiindex);
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_DELETEROI, (LONG)roiindex);
  return val;
}

int APIENTRY SelectRoi(DWORD roiindex)
    // selects ROI
{
  int val=0;
  HWND hwndClient = FIND_WINDOW("mpwframe-MCS8",NULL);
  if (hwndClient) 
    return (int)SendMessage(hwndClient, WM_COMMAND, ID_SELECTROI, roiindex);
  return val;
}

int APIENTRY GetRoiSum(DWORD roiindex, double *sum)
	// get sum of counts in ROI, 
	// returns roiindex= in LOWORD the spectra number, in HIWORD the roiid, or 0 if not found
{
  int val=0, nDisplay, roiid;
  if (!hwndServer) hwndServer = FIND_WINDOW("MCS8 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_GETROISUM, (LONG)roiindex);
  if (val) {
	  roiid = HIWORD(roiindex);
	  if (roiid < 100) nDisplay = LOWORD(roiindex);
	  else nDisplay = (roiid/100) - 1;
	  if(LVGetOneCnt(sum, nDisplay, 7)) return 0;
//	  if(LVGetOneCnt(area, nDisplay, 8)) return 0;
    // cnt[7] contains the sum, cnt[8] contains the area   
  }
  else *sum=0.;
  return val;
}

int APIENTRY LVGetRoinam(char *strp, int nDisplay)	// get Roi names
{
  int i;
  char sz[40];
  HANDLE hcm;
  char *roinam;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MCS8_RN_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(roinam = (char *)MapViewOfFile(hcm,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hcm);
    return 4;
  }
  for (i=0; i<4032; i++)		// (MAXREGIONS + MAXROI) * ROINAMELEN
    strp[i] = roinam[i];
  UnmapViewOfFile(roinam);
  CloseHandle(hcm);
  return 0;
}

