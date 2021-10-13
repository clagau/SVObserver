// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageTestApp.h
// * .File Name       : $Workfile:   SVImageTestApp.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:07:48  $
// ******************************************************************************

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "SVLibrary/InitialInformation.h"
#include "SVLibrary/SVOIniLoader.h"
#pragma endregion Includes

class SVTestAcquisitionSubsystem;

class SVImageTestApp : public CWinApp
{
public:
	SVImageTestApp();
	virtual ~SVImageTestApp();

	SvLib::SVOIniLoader m_iniLoader;
	std::string m_svimIniFile;
	std::string m_hardwareIniFile;
	std::string m_oemIniFile;

	bool IsGigeSystem() const;

	SVTestAcquisitionSubsystem* LoadDigitizer();
	void UnLoadDigitizer();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVImageTestApp)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVImageTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	SvLib::InitialInformation m_iniFileInfo;

	SVTestAcquisitionSubsystem* m_pSubsystem;
};

