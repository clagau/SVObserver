#pragma once

#ifdef SVLPTDLL_EXPORTS
#define SVLPTDLL_API __declspec(dllexport)
#else
#define SVLPTDLL_API __declspec(dllimport)
#endif

typedef void (*INTCALLBACKFUNC)(void);




SVLPTDLL_API HRESULT Open(void);
SVLPTDLL_API HRESULT Close(void);
SVLPTDLL_API bool IsDriverActive();
SVLPTDLL_API int GetDriverVersion();
SVLPTDLL_API HRESULT SupportsBidirectional(bool& supported);
SVLPTDLL_API HRESULT SetWriteMode();
SVLPTDLL_API HRESULT SetReadMode();
SVLPTDLL_API HRESULT DisableInterrupt();
SVLPTDLL_API HRESULT EnableInterrupt(INTCALLBACKFUNC fbCallback);
SVLPTDLL_API HRESULT ReadDataPort(unsigned char& databyte);
SVLPTDLL_API HRESULT WriteDataPort(unsigned char databyte);
SVLPTDLL_API HRESULT ReadStatusPort(unsigned char& statusbyte);
SVLPTDLL_API HRESULT ReadControlPort(unsigned char& controlbyte);
SVLPTDLL_API HRESULT WriteControlPort(unsigned char controlbyte);
SVLPTDLL_API int GetDllVersion();