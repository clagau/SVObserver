// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageTest.h
// * .File Name       : $Workfile:   SVImageTest.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:07:48  $
// ******************************************************************************

#ifndef SVIMAGETEST_H
#define SVIMAGETEST_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "SVTestAcquisitionSubsystem.h"
#include "SVOMFCLibrary/SVOIniLoader.h"

class CSVImageTestApp : public CWinApp
{
public:
	CSVImageTestApp();

	SVOIniLoader m_iniLoader;
	CString m_svimIniFile;
	CString m_hardwareIniFile;
	CString m_oemIniFile;

	bool IsDigitizerAnalog() const;
	bool IsGigeSystem() const;

	SVTestAcquisitionSubsystem* LoadDigitizer();
	void UnLoadDigitizer();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVImageTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSVImageTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	SVTestAcquisitionSubsystem* m_pSubsystem;

	bool ReadSVIMModelNo();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

