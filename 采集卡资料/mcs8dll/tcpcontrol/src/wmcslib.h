
extern "C" int     FAR PASCAL DetectWebmates();
extern "C" LPCTSTR FAR PASCAL DetectedWebmateIP(int idx);


extern "C" BOOL FAR PASCAL WebmateConnect(int idx, LPCTSTR IP);
extern "C" BOOL FAR PASCAL WebmateCmd(int idx, char *cmd);
extern "C" void FAR PASCAL WebmateDisconnect(int idx);

