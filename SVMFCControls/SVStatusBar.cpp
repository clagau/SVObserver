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
	if( nullptr != lpDrawItemStruct->hDC )
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
				case ID_INDICATOR_FBWF:
				{
					COLORREF l_oldBKColor = dc.SetBkColor( m_lFbwfBKColor );
					COLORREF l_oldForeColor = dc.SetTextColor( m_lFbwfForeColor );
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, m_strFbwfText );
					dc.SetBkColor( l_oldBKColor );
					dc.SetTextColor( l_oldForeColor );
					break;
				}
				default:
				{
					//do nothing 
					break;
				}
			}
		}
		dc.Detach();
	}
}


void SVStatusBar::SetFbwfInfo(UINT PaneIndex,bool FbwfAvailable, bool FbwfActive, bool FbwfChanging)
{
	if(FbwfAvailable)
	{				
		if(FbwfChanging)
		{
			m_lFbwfBKColor = FbwfActive ? RGB( 255, 200, 255) : RGB( 100, 100, 255);
			m_lFbwfForeColor = RGB( 0, 0, 0 );

			m_strFbwfText = FbwfActive ? "Disk protection will be inactive after reboot" : "Disk protection will be active after reboot";
		}
		else
		{
			m_lFbwfBKColor = FbwfActive ? RGB( 100, 255, 100) : RGB( 128, 128, 128); //Arvid temporarily displayed in grey as per SVO-1084. Color was: RGB( 255, 100, 100);
			m_lFbwfForeColor = RGB( 0, 0, 0 );

			m_strFbwfText = FbwfActive ? "Disk protection active" : "Disk protection inactive";
		}
	}
	else
	{
		m_lFbwfBKColor = RGB( 192, 128, 128 );
		m_lFbwfForeColor = RGB( 255, 255, 255);
		m_strFbwfText = "Disk protection not available!";
	}

	CClientDC DC( nullptr );
	int l_Width = DC.GetTextExtent(m_strFbwfText ).cx - (3*m_strFbwfText.GetLength())/2;
	SetPaneInfo( PaneIndex,ID_INDICATOR_FBWF,SBPS_DISABLED ,l_Width);
}


