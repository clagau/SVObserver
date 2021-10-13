//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .File Name       : $Workfile:   InitializeIOSubsystemApp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Sep 2014 11:35:20  $
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

class InitializeIOSubsystemApp : public CWinApp
{
public:
	InitializeIOSubsystemApp();
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

extern InitializeIOSubsystemApp theApp;
