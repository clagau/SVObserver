//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWinUtility
//* .File Name       : $Workfile:   SVWinUtility.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:51:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVWinUtility.h"

void SVYieldMessages(HWND hWnd /* = nullptr*/ )
{// Let other messages through
	MSG msg;

	while (::PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void SVYieldPaintMessages(HWND hWnd /* = nullptr */ )
{// Let other PAINT messages through
	
	MSG msg;
	
	while (::PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

