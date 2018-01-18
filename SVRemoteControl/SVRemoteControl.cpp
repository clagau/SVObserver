//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteControl
//* .File Name       : $Workfile:   SVRemoteControl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2014 10:59:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "resource.h"

#include "SVRemoteControl.h"

#include "SVSocketLibrary/SVSocketLibrary.h"
#include "dlldatax.h"
#include <google/protobuf/stubs/common.h>

#pragma endregion Includes

class CSVRemoteControlModule : public CAtlDllModuleT< CSVRemoteControlModule >
{
public :
	DECLARE_LIBID(LIBID_SVRemoteControlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SVREMOTECONTROL, "{AAD22C10-5AB0-46E6-BA78-90AF243C6166}")
};

CSVRemoteControlModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	BOOL l_Status = true;

	#ifdef _MERGE_PROXYSTUB
		l_Status = PrxDllMain( hInstance, dwReason, lpReserved );
	#endif

	if( l_Status )
	{
		hInstance;
		l_Status = _AtlModule.DllMain(dwReason, lpReserved); 
	}

	if( l_Status )
	{
		if( dwReason == DLL_PROCESS_ATTACH )
		{
			l_Status = (SvSol::SVSocketLibrary::Init() == SvSol::SVSocketError::Success );
		}
		else if( dwReason == DLL_PROCESS_DETACH )
		{
			SvSol::SVSocketLibrary::Destroy();
			google::protobuf::ShutdownProtobufLibrary();
		}
	}

	return l_Status;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}
