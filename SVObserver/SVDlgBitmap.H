//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgBitmapClass
//* .File Name       : $Workfile:   SVDlgBitmap.H  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:56  $
//******************************************************************************

#ifndef SVDLGBITMAP_H
#define SVDLGBITMAP_H

#include "SVContainerLibrary/SVVector.h"

struct SVDlgBitmapTextStruct
{
	SVDlgBitmapTextStruct()
	{
		BAbsCoord = TRUE;
		X = 0.0;
		Y = 0.0;
		Color = 0;
		StrText = _T( "" );
	};

	BOOL BAbsCoord;
	double X;
	double Y;
	COLORREF Color;
	CString StrText;
};




class SVDlgBitmapTextListClass : public SVVector< SVDlgBitmapTextStruct* >
{
public:
	SVDlgBitmapTextListClass() {};

	virtual ~SVDlgBitmapTextListClass() 
	{ 
		for( int i = GetSize() - 1; i >= 0; --i )
			if( GetAt( i ) )
				delete( GetAt( i ) );

		RemoveAll();
	}
};




class SVDlgBitmapClass : public CStatic
{
protected:
	//{{AFX_MSG(SVDlgBitmapClass)
	afx_msg void OnPaint();
	afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


public:
	SVDlgBitmapClass();
	virtual ~SVDlgBitmapClass();

	BOOL Init( LPCTSTR lpszResourceName );
	BOOL Init( UINT nIDResource );

	CSize GetDimensions();

	BOOL SetWindowSize( int cx = -1, int cy = -1 );
	CSize GetWindowSize();

	BOOL TextOut( int X, int Y, COLORREF Color, LPCTSTR StrText );
	BOOL TextOut( double X, double Y, COLORREF Color, LPCTSTR StrText );

public:
	BOOL OEMMode;

protected:
	HBITMAP InitFromFile();

	//{{AFX_VIRTUAL(SVDlgBitmapClass)
	//}}AFX_VIRTUAL	

protected:
	CBitmap		bitmap;
	SVDlgBitmapTextListClass textList;

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif	// SVDLGBITMAP_H














//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgBitmap.H_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:14:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Nov 2010 16:09:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   20 May 2005 12:28:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetWindowSize, SetWindowSize
 * cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 09:02:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

