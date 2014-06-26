//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TextObject
//* .File Name       : $Workfile:   TextObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:29:00  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "TextObject.h"
#include "SVPictureDisplayIdl.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
TextObject::TextObject()
{
}

TextObject::~TextObject()
{
}

TextObject::TextObject( TextObject& p_rhs )
{
	*this = p_rhs;
}

TextObject::TextObject( long x, long y, COLORREF color, LPCTSTR text, long lAllowEdit )
{
	m_pos.x = x;
	m_pos.y = y;
	m_color = color;
	m_SelectedColor = color;
	m_text = text;
	SetEditAllowed( lAllowEdit );
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT TextObject::SetText(long x, long y, LPCTSTR text)
{
	m_pos.x = x;
	m_pos.y = y;
	m_text = text;
	return S_OK;
}

TextObject& TextObject::operator=( TextObject& p_rhs )
{
	if( this != &p_rhs )
	{
		m_pos = p_rhs.m_pos;
		m_color = p_rhs.m_color;
		m_text = p_rhs.m_text;
	}
	return *this;
}

void TextObject::Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	CFont l_Font;
	l_Font.CreatePointFont( static_cast<int>(120 * p_fZoomHeight), _T("Arial"), &rDC );
	COLORREF l_OldColor;
	if( p_bSelected )
	{
		l_OldColor = rDC.SetTextColor( m_SelectedColor );
	}
	else
	{
		l_OldColor = rDC.SetTextColor( m_color );
	}

	CFont* l_pOldFont = rDC.SelectObject( &l_Font );
	int l_OldBkMode = rDC.SetBkMode( TRANSPARENT );

	rDC.TextOut( 
		static_cast<long>((m_pos.x - p_dOffset.x) * p_fZoomWidth), 
		static_cast<long>((m_pos.y - p_dOffset.y) * p_fZoomHeight), 
		m_text, 
		m_text.GetLength() );

	rDC.SetBkMode( l_OldBkMode );
	rDC.SelectObject( l_pOldFont );
	l_Font.DeleteObject();
	rDC.SetTextColor( l_OldColor );
}

bool TextObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	CRect l_rec( m_pos.x - GRAB_SIZE, m_pos.y - GRAB_SIZE, m_pos.x + GRAB_SIZE, m_pos.y + GRAB_SIZE );
	if( l_rec.PtInRect( imagePoint ) )
	{
		bRet = true;
		SelType = HTOBJECT;
	}
	return bRet;
}

bool TextObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	bool retVal = false;
	switch (SelType)
	{
	case HTBOTTOMLEFT :
		m_pos.x += imageMovePoint.x;
		m_pos.y += imageMovePoint.y;
		retVal = true;
		break;
	case HTBOTTOMRIGHT:
		m_pos.x += imageMovePoint.x;
		m_pos.y += imageMovePoint.y;
		retVal = true;
		break;
	case HTTOPLEFT:
		m_pos.x += imageMovePoint.x;
		m_pos.y += imageMovePoint.y;
		retVal = true;
		break;
	case HTTOPRIGHT:
		m_pos.x += imageMovePoint.x;
		m_pos.y += imageMovePoint.y;
		retVal = true;
		break;
	case HTOBJECT:
		m_pos.x += imageMovePoint.x;
		m_pos.y += imageMovePoint.y;
		retVal = true;
		break;
	default:
		break;
	}

	if (retVal)
	{
		MoveChild(SelType,  imageMovePoint, viewMovePoint);
	}

	return retVal;
}

void TextObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
		{
		case P_Type:
			ASSERT( TextROI == parameterValue.lVal );
			break;
		case P_X1:
			m_pos.x = parameterValue.lVal;
			break;
		case P_Y1:
			m_pos.y = parameterValue.lVal;
			break;
		case P_Text:
			m_text = parameterValue.bstrVal;
			break;
		default:
			DrawObject::SetParameter(parameterId, parameterValue);
			break;
		}
}

void TextObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(TextROI);
	ParameterMap[P_X1] = m_pos.x;
	ParameterMap[P_Y1] = m_pos.y;
	ParameterMap[P_Text] = m_text;
}

RECT TextObject::GetRectangle() const
{
	SIZE size;
	size.cx = 10;		// this is temporary
	size.cy = 10;		// We need to know about the current font and zoom etc.

	CRect lRec( m_pos, size);
	return lRec;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\TextObject.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:29:00   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jun 2010 11:58:56   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  8
 * SCR Title:  Font Training
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated Picture Control to be able to used a different color for selected objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 May 2010 07:06:50   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed move functions to return true if they can be resized or moved. There is now a limit on minimum size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:26:06   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/