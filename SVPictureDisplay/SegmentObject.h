//*****************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a segment object.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "EllipseObject.h"
#pragma endregion Includes

class SegmentObject : public EllipseObject
{
public:
#pragma region Constructor
	SegmentObject();
	virtual ~SegmentObject();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw(POINT p_dOffset, double p_dZoomWidth, double p_dZoomHeight, CDC& rDC, bool p_bSelected) override;
	virtual bool IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const override;
	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint) override;

	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
#pragma endregion Public Methods

#pragma region Member Variables
private:
	double m_startAngle = 0;
	double m_stopAngle = 360;

	mutable CPoint m_moveStartPoint;
#pragma endregion Member Variables
};

