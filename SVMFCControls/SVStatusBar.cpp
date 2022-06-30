//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/Color.h"

SVStatusBar::SVStatusBar()
	:m_lModeBKColor {SvDef::LightGray}
	,m_lModeForeColor {SvDef::Black}
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


