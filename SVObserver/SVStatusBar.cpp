//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatusBar
//* .File Name       : $Workfile:   SVStatusBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:06:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVStatusBar.h"

SVStatusBar::SVStatusBar()
{
	m_lModeBKColor = RGB( 192, 192, 192 );
	m_lModeForeColor = RGB( 0, 0, 0 );
}

SVStatusBar::~SVStatusBar()
{
}

void SVStatusBar::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if( lpDrawItemStruct->hDC != NULL )
	{
		CDC dc;
		dc.Attach( lpDrawItemStruct->hDC );

		// Change Color based on indicator...
		if( lpDrawItemStruct->itemID < sizeof( indicators ) / sizeof( UINT ) )
		{
			switch ( indicators[ lpDrawItemStruct->itemID ] )
			{
				case ID_INDICATOR_MODE: 
				{
					COLORREF l_oldBKColor = dc.SetBkColor( m_lModeBKColor );
					COLORREF l_oldForeColor = dc.SetTextColor( m_lModeForeColor );
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, m_strModeText );
					dc.SetBkColor( l_oldBKColor );
					dc.SetTextColor( l_oldForeColor );
					break;
				}
				case ID_INDICATOR_GIGE_LICENSE_ERROR:
				{
					COLORREF l_oldBKColor = dc.SetBkColor( m_lErrorBKColor );
					COLORREF l_oldForeColor = dc.SetTextColor( m_lErrorForeColor );
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, m_strErrorText );
					dc.SetBkColor( l_oldBKColor );
					dc.SetTextColor( l_oldForeColor );

					break;
				}
				default:
				{
				}
			}
		}
		dc.Detach();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStatusBar.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:06:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Feb 2013 11:26:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added case for ID_INDICATOR_GIGE_LICENSE_ERROR to display all license errors in the status bar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jul 2010 10:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed to use default Device Context instead of status bar Device Context.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jul 2010 09:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status bar functionality to better size the data so that it does not scroll off the screen.  Updated version number to 5.32 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2005 11:48:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug with status bar text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Aug 2005 09:07:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Mode on StatusBar and Add color indicators.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
