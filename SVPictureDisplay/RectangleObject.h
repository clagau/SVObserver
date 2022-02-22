//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RectangleObject
//* .File Name       : $Workfile:   RectangleObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#pragma endregion Includes

class RectangleObject : public DrawObject
{
public:
#pragma region Constructor
	RectangleObject();
	RectangleObject( long left, long top, long right, long bottom, COLORREF color, long lAllowEdit );
	virtual ~RectangleObject();
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT SetRectangle(long left, long top, long right, long bottom);
	virtual void Draw( POINT p_dOffset, double p_dZoomWidth, double p_dZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Member Variables
protected:
	CRect m_rect;  // Needs to be protected because EllipseObject is derived from this class.
#pragma endregion Member Variables
};

