//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplay
//* .File Name       : $Workfile:   SVPictureDisplay.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVPictureDisplay.h"

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVPictureDisplayApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xFEF66C91, 0x65ED, 0x4FDA, { 0xBA, 0x70, 0x79, 0x77, 0x2, 0x35, 0x83, 0x58 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

BOOL SVPictureDisplayApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	return bInit;
}

int SVPictureDisplayApp::ExitInstance()
{
	return COleControlModule::ExitInstance();
}

// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	return NOERROR;
}

// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	return NOERROR;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDisplay.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:28:04   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:20   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/