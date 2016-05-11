//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceApp
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceApp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:46  $
//******************************************************************************
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SVFileAcquisitionDevice.h"

/////////////////////////////////////////////////////////////////////////////
// SVFileAcquisitionDeviceApp
// See SVFileAcquisitionDeviceApp.cpp for the implementation of this class
//
class SVFileAcquisitionDeviceApp : public CWinApp
{
public:
	SVFileAcquisitionDeviceApp();
	virtual ~SVFileAcquisitionDeviceApp();
	SVFileAcquisitionDevice m_fileAcqDevice;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFileAcquisitionDeviceApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVFileAcquisitionDeviceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern SVFileAcquisitionDeviceApp g_svTheApp;
