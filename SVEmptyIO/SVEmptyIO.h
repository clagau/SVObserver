//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEmptyIO
//* .File Name       : $Workfile:   SVEmptyIO.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:13:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
#pragma endregion Includes

// CSVEmptyIOApp
// See SVEmptyIO.cpp for the implementation of this class
//

class CSVEmptyIOApp : public CWinApp
{
public:
	CSVEmptyIOApp();
	virtual ~CSVEmptyIOApp();

// Overrides
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

