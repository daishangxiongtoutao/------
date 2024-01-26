#define INTL
#include <windows.h>
#include <stdio.h>
#include "wmcslib.h"

#define MAXDEV	16

typedef struct{
  int connected;
  int demomod;
  char ipaddress[20];
} BOARDPROPERTY;



int localpcs=0, nDev=0, mcano=0;
int nDetected=0;
BOARDPROPERTY Board[16] = {0};


void help()
{
	printf("Commands:\n");
	printf("H			Help\n");
	printf("winexec program		run windows program\n");
	printf("end			end connection\n");	
	printf("bye			terminate tcp server\n");	
	printf("Q			Quit and terminate tcp server\n");
	printf("?			Show Status\n");
	printf("exit			exit MPANT server program\n");	
	printf("start			Start acquisitiom\n");
	printf("halt			Halt acquisitiom\n");
	printf("erase			Erase spectra\n");
    printf("(... more see command language in the manual)\n");
    printf("\n");
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

int run(char *command)
{
	int i;
	char cmd[4096];
	if (!stricmp(command, "H"))           help();

	else if (!stricmp(command,"Q")) { 
	  strcpy(command, "bye");
	  for (i=0; i<nDetected; i++)
	    WebmateCmd(i, command);
		return 1;
	}
	else {
	  for (i=0; i<nDetected; i++) {
		if (strlen(command)) {
	      strncpy(cmd, command, 4095);
		  WebmateCmd(i, cmd);
		  if (nDetected > 1) printf("Server %d:\n", i);
		  printf("%s\n", cmd);
		}
	  }
	}
	return 0;
}

int config(char *item, int nDev)
{
  int j;
  int ret=0;

  if (item[0] == ';') return ret;
  if (nDev == -1) {
    if (!strnicmp("localpcs=", item, 9)) {
      sscanf(item+9,"%d",&localpcs);
      if (localpcs > MAXDEV) localpcs = MAXDEV;
      if (localpcs < 1) localpcs = 1;

	  mcano=0;
	  for (j=0; j<localpcs; j++) 
		  Board[j].ipaddress[0] = '\0';
	  return ret;
    }
	else if(!strnicmp("ip=", item, 3)) {
	  if (mcano >= localpcs) return ret;
	  sscanf(item+3,"%s", Board[mcano].ipaddress);
	  mcano++;
	  return ret;
	}
  }
  else
    ret=1;
  return ret;
}

int freadstr(FILE *stream, char *buff, int buflen)
{
  int i=0,ic;

  while ((ic=getc(stream)) != 10) {
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

int readcnf(LPSTR cnfnm, int nDev)
{
  int nDev0=-1;
  FILE *filhnd;
  char item[256];

  if (!(filhnd = fopen(cnfnm,"rb"))) return 1;
  while(!freadstr(filhnd,item,256)) {
    config(item, nDev0); 
  }
  fclose(filhnd);
  return 0;
}

int cardinit()
{
  int nDev;
  int connected=0;
  int search=0;

  if (!localpcs) localpcs=4;
  
  for (nDev=0; nDev<localpcs; nDev++) {
	if (Board[nDev].ipaddress[0] == '\0') {
	  search=1;
	  break;
	}
  }

  if (search) {
	  nDetected=DetectWebmates();
	  printf("%d boards detected\n", nDetected);
	  localpcs = nDetected;
  }
  else nDetected=localpcs;
  if (nDetected>MAXDEV) nDetected=MAXDEV;

  for (nDev=0; nDev<nDetected; nDev++) {
    Board[nDev].connected = 0;
    if (nDev >= localpcs) continue;
	if (Board[nDev].ipaddress[0] == '\0')
	  strcpy(Board[nDev].ipaddress, DetectedWebmateIP(nDev));
	printf("Try to connect %s...",Board[nDev].ipaddress); 

	if (WebmateConnect(nDev, Board[nDev].ipaddress)) {
      Board[nDev].connected=1;
	  printf("successfull\n");
	  connected++;
    }
	else {
		printf("failed\n");
		Board[nDev].demomod=1;
	}
  }
  for (; nDev < localpcs; nDev++) {
    Board[nDev].connected=0;
	Board[nDev].demomod=1;
  }
  return connected;
}

int _cdecl main(int argc, char** argv)
{
  int         i, bsave=0, connected=0;
  FILE *filhnd;

//  BOOL        bConnected[16];
  char command[4096]={0};

//  int         nRunning, k;
  localpcs = 0;
  if (readcnf("wmcs.ini", nDev)) bsave=1;

  if (bsave) 
    printf("Detecting MCS Servers ... ");
  connected = cardinit();
  if ( connected && bsave) {

    // 
    filhnd = fopen("wmcs.ini", "wt");
    printf("Detected IP's: \n");
	fprintf(filhnd, "localpcs=%d\n", nDetected);
    for (i=0; i<nDetected; i++) {
      printf("ip=%s\n", DetectedWebmateIP(i));
	  fprintf(filhnd, "ip=%s\n", DetectedWebmateIP(i));
    }
    printf("\n\n");
    fclose(filhnd);
	mcano = connected;
  }
  
  if (!mcano) {
	  printf("no localpcs!\n");
	  goto out;
  }

  help();

  while(TRUE)
	{
		readstr(command, 80);
		if (run(command)) break;
	}

  // Disconnect
  for (i=0; i<nDetected; i++) if (Board[i].connected) {
    WebmateDisconnect(i);
    Board[i].connected=0;
  }

out:
  Sleep(4000);
  return 0;
}

