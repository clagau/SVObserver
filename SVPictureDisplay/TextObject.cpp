//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
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

TextObject::TextObject( const TextObject& p_rhs )
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

TextObject& TextObject::operator=( const TextObject& p_rhs )
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

bool TextObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& ) const
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
			Log_Assert( TextROI == parameterValue.lVal );
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

