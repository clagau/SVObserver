// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#pragma endregion Includes

#pragma comment( lib, "mil" )
#pragma comment( lib, "milim" )

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditor : See SVMaskEditor.cpp for implementation.

class SVMaskEditor : public COleControlModule
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
