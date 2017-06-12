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

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "SVTestAcquisitionSubsystem.h"
#include "SVLibrary/InitialInformation.h"
#include "SVLibrary/SVOIniLoader.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class CSVImageTestApp : public CWinApp
{
public:
	CSVImageTestApp();
	virtual ~CSVImageTestApp();

	SvLib::SVOIniLoader m_iniLoader;
	SVString m_svimIniFile;
	SVString m_hardwareIniFile;
	SVString m_oemIniFile;

	bool IsGigeSystem() const;

	SVTestAcquisitionSubsystem* LoadDigitizer();
	void UnLoadDigitizer();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVImageTestApp)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSVImageTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	SvLib::InitialInformation m_iniFileInfo;

	SVTestAcquisitionSubsystem* m_pSubsystem;

	bool ReadSVIMModelNo();
};

