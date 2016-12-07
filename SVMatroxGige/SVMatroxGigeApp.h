//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeApp
//* .File Name       : $Workfile:   SVMatroxGigeApp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SVMatroxGige.h"
#pragma endregion Includes

class CSVMatroxGigeApp : public CWinApp
{
public:
	SVMatroxGige m_svSystem;
	
	CSVMatroxGigeApp();
	virtual ~CSVMatroxGigeApp();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVMatroxGigeApp)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSVMAtroxGigeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSVMatroxGigeApp g_svTheApp;
