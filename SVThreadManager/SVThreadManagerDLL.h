//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadManager
//* .File Name       : $Workfile:   SVThreadManagerDLL.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   01 Dec 2014 11:21:46  $
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSVThreadManagerApp
// See SVThreadManager.cpp for the implementation of this class
//

class CSVThreadManagerApp : public CWinApp
{
#pragma region Public
public:
	CSVThreadManagerApp();
	virtual ~CSVThreadManagerApp();
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
#pragma endregion
};

