//*****************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a triangle object.
//*****************************************************************************
#pragma once

#pragma region Includes
#include <array>
#include "DrawObject.h"
#pragma endregion Includes

class TriangleObject : public DrawObject
{
public:
#pragma region Constructor
	TriangleObject();
	virtual ~TriangleObject();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw(POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected) override;
	virtual bool IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const override;
	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Member Variables
protected:
	std::array<POINT, 3> m_points;
#pragma endregion Member Variables
};

