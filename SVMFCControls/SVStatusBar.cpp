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
#include "SVStatusLibrary/EditLock.h"

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
					COLORREF oldBKColor = dc.SetBkColor(m_lModeBKColor);
					COLORREF oldForeColor = dc.SetTextColor(m_lModeForeColor);
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, m_strModeText);
					dc.SetBkColor(oldBKColor);
					dc.SetTextColor(oldForeColor);
					break;
				}
				case ID_INDICATOR_EDIT_LOCK:
				{

					bool isLockAcquired = SvStl::EditLock::isAcquiredBySVIM();
					std::string EditLockInfo {_T(isLockAcquired ? "   Edit Lock   " : "no Edit Lock")};

					COLORREF oldBKColor = dc.SetBkColor(isLockAcquired ? SvDef::DarkYellow : SvDef::StatusBarBackground);
					COLORREF oldForeColor = dc.SetTextColor(SvDef::Black);
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, EditLockInfo.c_str());
					dc.SetBkColor(oldBKColor );
					dc.SetTextColor(oldForeColor );
					break;
				}
				case ID_INDICATOR_GIGE_LICENSE_ERROR:
				{
					COLORREF oldBKColor = dc.SetBkColor( m_lErrorBKColor );
					COLORREF oldForeColor = dc.SetTextColor( m_lErrorForeColor );
					dc.TextOut(lpDrawItemStruct->rcItem.left + 1, lpDrawItemStruct->rcItem.top, m_strErrorText );
					dc.SetBkColor(oldBKColor );
					dc.SetTextColor(oldForeColor );

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


