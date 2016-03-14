//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LineObject
//* .File Name       : $Workfile:   LineObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#pragma endregion Includes

class LineObject : public DrawObject
{
public:
#pragma region Constructor
	LineObject();
	LineObject( long x1, long y1, long x2, long y2, COLORREF color, long lAllowEdit );
	virtual ~LineObject();
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT SetLine(long x1, long y1, long x2, long y2);
	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual void GetLine( POINT& p_rStart, POINT& p_rEnd ) const;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Member Variables
protected:
	POINT m_StartPos;
	POINT m_EndPos;
#pragma endregion Member Variables
};

