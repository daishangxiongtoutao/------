#define MAXSCDEV   4
#define NUMSCALER  6
#define SCALNAMLEN 60

typedef struct{
  int active;           // 0=not used, 1..4 system no. 
  int ratemeter;
  int scupdaterate;
  int started;

  int scalnum;         // no. of used scalers 1..6
  int contrl;          // count_control 
						// bit 0: reserved, count capture
						// bit 1: Common Gate Polarity, 1=active High
						// bit 2: Extern Clear polarity, 1=active High
						// bit 3: Preset CNT1
						// bit 4: Preset CNT2 
						//
  int presetena;		// bit 0..1 enables respective preset
  int timermode;		// bit 0..7 enable 8 ns timer for respective scaler
  int polarity;			// bit 0..7 1=rising / 0=falling edge counting for respective counter
  int gowatch;			// bit 0..7 GO line low halts respective counter
  int arm;				// bit 0..7 enables respective counter
  int loadena;			// bit 0..7 enables extern preset load for respective counter
  int commongate;		// bit 0..7 enables common gate use for respective counter
  int gateena;			// bit 0,1, 4..7: 1= individual gate enable for respective counter
						//          0=always count up for up/down counter    
						// bit 8..15 1=up/dn counting for resp. scaler 0..7
  int gatepol;			//  1=active high gate enables counting for respective counter
						//  1=count up / 0=count down if gate high for up/down counter

  __int64 preset[2];    // presets for counter 0..1
  double offset[NUMSCALER+3];  // offset[0]+scalval[0] is counter value
						// offset[NUMSCALER] is -(time offset)
  char name[NUMSCALER*SCALNAMLEN];
} SCSETTING;

typedef struct {
  double max[NUMSCALER]; // for ratemeter
  int bar[4];			 // selects scaler for scrollbar
  int x;                 // Window Pos
  int y;
} SCINFO;
