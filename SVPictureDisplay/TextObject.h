//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TextObject
//* .File Name       : $Workfile:   TextObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:29:00  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "drawobject.h"
#pragma endregion Includes

class TextObject : public DrawObject
{
public:
#pragma region Constructor
	TextObject();
	TextObject( TextObject& p_rhs );
	TextObject( long x, long y, COLORREF color, LPCTSTR text, long lAllowEdit );
	virtual ~TextObject();
#pragma endregion Constructor

#pragma region Public Methods
	TextObject& operator=( TextObject& p_rhs );
	HRESULT SetText(long x, long y, LPCTSTR text);

	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;

	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Member Variables
private:
	POINT m_pos;
	CString m_text;
#pragma endregion Member Variables
};

