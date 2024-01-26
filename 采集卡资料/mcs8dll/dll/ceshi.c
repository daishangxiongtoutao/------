//-----------------------------------------
//测试MCS8A的单通道采集
//功能：16通道数据采集积分值并按照时间和通道数命名txt采集
//--------------------zhaoyanpeng

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#undef DLL
#include "STRUCT.H"
#include "dmcs8.h"

int APIENTRY GetStatusData(ACQSTATUS  *Status, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bStatus[nDisplay]) {
    _fmemcpy((LPSTR  *)Status,
           (LPSTR  *)&DLLStatus[nDisplay],sizeof(ACQSTATUS));
  }
  return bStatus[nDisplay];
}

