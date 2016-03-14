//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplay
//* .File Name       : $Workfile:   SVPictureDisplay.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols
#pragma endregion Includes

#pragma region Declarations
extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
#pragma endregion Declarations

class SVPictureDisplayApp : public COleControlModule
{
#pragma region Public Methods
public:
	BOOL InitInstance();
	int ExitInstance();
#pragma endregion Public Methods
};

