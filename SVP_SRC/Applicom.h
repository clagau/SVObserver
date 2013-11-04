/*****************************************************************************
*                                                                            *
*   APPLICOM.H              Version : 3.2.0              date :14/05/98      *
*                                                                            *
*     Copyright (c) 1998 applicom international. All rights reserved.        *
*                                                                            *
*     Include Declarations file for Microsoft C language applications        *
*                                                                            *
*                 ******************************************                 *
*                 *   WARNING   do not modify this file    *                 *
*                 ******************************************                 *
*                                                                            *
******************************************************************************/
#define APP_256BYTES_BASED_LIMITS 256
#define APP_1584BYTES_BASED_LIMITS 1584

void WINAPI initbus(short int*);
void WINAPI exitbus(short int*);
void WINAPI masterinfo(short int*,short int*,char*,short int*);
void WINAPI readpackbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readpackibit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readpackqbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readbyte(short int*,short int*,short int*,long int*,short int*,short int*);
void WINAPI readibyte(short int*,short int*,short int*,long int*,short int*,short int*);
void WINAPI readqbyte(short int*,short int*,short int*,long int*,short int*,short int*);
void WINAPI readpackbyte(short int*,short int*,short int*,long int*,char*,short int*);
void WINAPI readpackibyte(short int*,short int*,short int*,long int*,char*,short int*);
void WINAPI readpackqbyte(short int*,short int*,short int*,long int*,char*,short int*);
void WINAPI readword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readiword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readqword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readwordbcd(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readdword(short int*,short int*,short int*,long*,long*,short int*);
void WINAPI readfword(short int*,short int*,short int*,long*,float*,short int*);
void WINAPI writepackbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writepackqbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writebyte(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writeqbyte(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writepackbyte(short int*,short int*,short int*,long*,char*,short int*);
void WINAPI writepackqbyte(short int*,short int*,short int*,long*,char*,short int*);
void WINAPI writeword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writeqword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writewordbcd(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writedword(short int*,short int*,short int*,long*,long*,short int*);
void WINAPI writefword(short int*,short int*,short int*,long*,float*,short int*);
void WINAPI readquickbit(short int*,short int*,short int*,short int*);
void WINAPI readdiag(short int*,short int*,short int*,short int*);
void WINAPI readeven(short int*,short int*,short int*,short int*);
void WINAPI readtrace(short int*,short int*,short int*,short int*);
void WINAPI readident(short int*,short int*,short int*,char*,short int*);
void WINAPI manual(short int*,short int*,short int*);
void WINAPI automatic(short int*,short int*,short int*);
void WINAPI AppConnect(short int*,short int*,short int*);
void WINAPI AppUnconnect(short int*,short int*,short int*);
void WINAPI statjbus(short int*,short int*,short int*);
void WINAPI iocounter(short int*,short int*,short int*,short int*);
void WINAPI resetiocounter(short int*,short int*,short int*);
void WINAPI readdifbit(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifibit(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifqbit(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifbyte(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifibyte(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifqbyte(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifword(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifiword(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifqword(short int*,short int*,short int*,long*,short int*);
void WINAPI readdifdword(short int*,short int*,short int*,long*,short int*);
void WINAPI readdiffword(short int*,short int*,short int*,long*,short int*);
void WINAPI writedifpackbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writedifpackqbit(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writedifpackbyte(short int*,short int*,short int*,long*,char*,short int*);
void WINAPI writedifpackqbyte(short int*,short int*,short int*,long*,char*,short int*);
void WINAPI writedifword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writedifqword(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writedifdword(short int*,short int*,short int*,long*,long*,short int*);
void WINAPI writediffword(short int*,short int*,short int*,long*,float*,short int*);
void WINAPI readdifquickbit(short int*,short int*,short int*);
void WINAPI transdif(short int*,short int*,short int*,void*,short int*);
void WINAPI transdifpack(short int*,short int*,short int*,void*,short int*);
void WINAPI testtransdif(short int*,short int*,short int*,short int*);
void WINAPI createcyc(short int*,short int*,short int*,short int*,short int*,short int*,short int*,short int*,long*,short int*,short int*,short int*);
void WINAPI startcyc(short int*,short int*,short int*);
void WINAPI stopcyc(short int*,short int*,short int*);
void WINAPI actcyc(short int*,short int*,short int*);
void WINAPI transcyc(short int*,short int*,short int*,short int*,void*,short int*);
void WINAPI transcycpack(short int*,short int*,short int*,short int*,void*,short int*);
void WINAPI CycExecuted(short int *,short int *,short int *,short int *);
void WINAPI AppGetCycParam(short int *,short int *,long int *, short int *);
void WINAPI getpackbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI getbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI getpackbyte(short int*,short int*,short int*,char*,short int*);
void WINAPI getword(short int*,short int*,short int*,short int*,short int*);
void WINAPI getdword(short int*,short int*,short int*,long*,short int*);
void WINAPI getfword(short int*,short int*,short int*,float*,short int*);
void WINAPI setpackbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI setbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI setpackbyte(short int*,short int*,short int*,char*,short int*);
void WINAPI setword(short int*,short int*,short int*,short int*,short int*);
void WINAPI setdword(short int*,short int*,short int*,long*,short int*);
void WINAPI setfword(short int*,short int*,short int*,float*,short int*);
void WINAPI getdispbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI getdispword(short int*,short int*,short int*,short int*,short int*);
void WINAPI getdispdword(short int*,short int*,short int*,long*,short int*);
void WINAPI getdispfword(short int*,short int*,short int*,float*,short int*);
void WINAPI setdispbit(short int*,short int*,short int*,short int*,short int*);
void WINAPI setdispword(short int*,short int*,short int*,short int*,short int*);
void WINAPI setdispdword(short int*,short int*,short int*,long*,short int*);
void WINAPI setdispfword(short int*,short int*,short int*,float*,short int*);
void WINAPI incdispword(short int*,short int*,short int*,short int*,short int*);
void WINAPI decdispword(short int*,short int*,short int*,short int*,short int*);
void WINAPI incdispdword(short int*,short int*,short int*,long*,short int*);
void WINAPI decdispdword(short int*,short int*,short int*,long*,short int*);
void WINAPI confdb(short int*,short int*,short int*,short int*,short int*,short int*);
void WINAPI getevent(short int*,short int*,short int*,short int*,long*,short int*,short int*,short int*);
void WINAPI setevent(short int*,short int*,short int*,short int*,short int*);
void WINAPI transwordbit(short int*,short int*,short int*,short int*);
void WINAPI transbitword(short int*,short int*,short int*,short int*);
void WINAPI compword(short int*,short int*,short int*,short int*,short int*);
void WINAPI invbit(short int*,short int*,short int*,short int*);
void WINAPI unpackdate(short int*,short int*,short int*,short int*,short int*);
void WINAPI unpacktime(short int*,short int*,short int*,short int*,short int*);
void WINAPI bcdbin(short int*,short int*,short int*,short int*);
void WINAPI binbcd(short int*,short int*,short int*,short int*);
void WINAPI writemes(short int*,short int*,char*,short int*);
void WINAPI readmes(short int*,short int*,short int*,short int*,short int*,char*,short int*);
void WINAPI writereadmes(short int*,short int*,char*,short int*,short int*,short int*,short int*,char*,short int*);
void WINAPI writedifmes(short int*,short int*,char*,short int*);
void WINAPI readdifmes(short int*,short int*,short int*,short int*,short int*,short int*);
void WINAPI writereaddifmes(short int*,short int*,char*,short int*,short int*,short int*,short int*,short int*);
void WINAPI getmodem(short int*,short int*,short int*,short int*);
void WINAPI setmodem(short int*,short int*,short int*,short int*);
void WINAPI watchdog(short int*,short int*,short int*);
void WINAPI accesskey(short int*,short int*,short int*,short int*);
void WINAPI txtute(short int*,short int*,short int*,short int*,char*,char*,short int*,short int*,short int*);
void WINAPI readdiagute(short int*,short int*,short int*,short int*);
void WINAPI resetcounterute(short int*,short int*,short int*);
void WINAPI readiobitute(short int*,short int*,short int*,short int*,short int*);
void WINAPI writeiobitute(short int*,short int*,short int*,short int*,short int*,short int*);
void WINAPI readtimerute(short int*,short int*,short int*,short int*,short int*);
void WINAPI readmonostableute(short int*,short int*,short int*,short int*,short int*);
void WINAPI readcounterute(short int*,short int*,short int*,short int*,short int*);
void WINAPI writetimerute(short int*,short int*,short int*,short int*,short int*);
void WINAPI writemonostableute(short int*,short int*,short int*,short int*,short int*);
void WINAPI writecounterute(short int*,short int*,short int*,short int*,short int*);
void WINAPI readtimer(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writetimer(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI readcounter(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI writecounter(short int*,short int*,short int*,long*,short int*,short int*);
void WINAPI AppIniTime(short int*,short int*);
void WINAPI AppFmsGetOd(short int*,short int*,short int*,short int*,short int*,
                 short int*,char*,short int*,short int*);
void WINAPI AppFmsStatus(short int*,short int*,unsigned short int*,unsigned short int*,short int*);
void WINAPI AppGetWatchDog(short int*,unsigned short int*,unsigned short int*,short int*);

void WINAPI EquipmentInfo(short int*,short int*,short int*,short int*);

BOOL WINAPI AuInitBuffer(unsigned short int, unsigned long*);
void WINAPI InitBuffer(unsigned short int*, short int*);
BOOL WINAPI AuMonGetSizeOfMail(unsigned short,unsigned short int*, unsigned short int*,
                               unsigned short int*,unsigned short int*, unsigned short int*, unsigned long*);
BOOL WINAPI AuSetApplicationMaxSize(short int,unsigned long*);
BOOL WINAPI AuGetApplicomMaxSize(unsigned short  wSizeDB[8],unsigned short wSizeChannel[32],unsigned long*);


