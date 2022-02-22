//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EllipseObject
//* .File Name       : $Workfile:   EllipseObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "RectangleObject.h"
#pragma endregion Includes

class EllipseObject : public RectangleObject
{
public:
#pragma region Constructor
	EllipseObject();
	virtual ~EllipseObject();

	EllipseObject( long left, long top, long right, long bottom, COLORREF color, long lAllowEdit );
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT SetEllipse(long left, long top, long right, long bottom);

	virtual void Draw( POINT p_dOffset, double p_dZoomWidth, double p_dZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;

	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
#pragma endregion Public Methods
};

