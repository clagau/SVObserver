//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingApp
//* .File Name       : $Workfile:   SVOCRFontTraining.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:37:08  $
//******************************************************************************

#if !defined(AFX_SVOCRFONTTRAINING_H__9AEFBD6F_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRFONTTRAINING_H__9AEFBD6F_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

extern int gError;
#define __GLOBAL_ERROR_CALLBACKS         // 11 Aug 1999 - frb.

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingApp : See SVOCRFontTraining.cpp for implementation.

class CSVOCRFontTrainingApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRFONTTRAINING_H__9AEFBD6F_F061_11D2_A7B2_00106F000C7D__INCLUDED)

