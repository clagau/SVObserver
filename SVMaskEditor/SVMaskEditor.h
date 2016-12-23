// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMaskEditor
// * .File Name       : $Workfile:   SVMaskEditor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   16 Jan 2014 10:31:38  $
// ******************************************************************************

#pragma once

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#pragma region Includes
#include "resource.h"       // main symbols

#include "CameraLibrary\SVGraphix.h"   // Loacted in SVObserver Source Code!
#include "SVMatroxLibrary\SVMatroxLibrary.h"
#pragma endregion Includes

#pragma comment( lib, "mil" )
#pragma comment( lib, "milim" )

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorApp : See SVMaskEditor.cpp for implementation.

class SVMaskEditorApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;


// Global Functions...
SVMatroxBuffer SVBitmapToMilBuffer( HBITMAP HBM );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
