//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
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

void SVYieldMessages(HWND hWnd /* = NULL*/ )
{// Let other messages through
	MSG msg;

	while (::PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void SVYieldPaintMessages(HWND hWnd /* = NULL */ )
{// Let other PAINT messages through
	
	MSG msg;
	
	while (::PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVWinUtility.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Sep 2005 16:40:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved implementation of SVYieldMessages and SVYieldPaintMessages to .cpp file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/