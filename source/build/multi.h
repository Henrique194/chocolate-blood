#pragma once

void initmultiplayers(char damultioption, char dacomrateoption, char dapriority);
void sendlogon();
void sendlogoff();
short getpacket(short *otherconnectindex, char *bufptr);
void uninitmultiplayers();
void sendpacket(short otherconnectindex, char* bufptr, short messleng);
int getoutputcirclesize();
