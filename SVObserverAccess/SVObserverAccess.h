//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SVObserverAccess.h
/// All Rights Reserved 
//*****************************************************************************
/// Defines the exported functions for the DLL application.
//******************************************************************************

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SVOBSERVERACCESS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SVOBSERVERACCESS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SVOBSERVERACCESS_EXPORTS
#define SVOBSERVERACCESS_API __declspec(dllexport)
#else
#define SVOBSERVERACCESS_API __declspec(dllimport)
#endif



namespace ObserverAccess
{
	extern SVOBSERVERACCESS_API int DefaultPort;
	SVOBSERVERACCESS_API void ExecuteRunReServer(unsigned int port);
	SVOBSERVERACCESS_API bool  SharedResourcesOk(bool bCheckSizeOverride);
}