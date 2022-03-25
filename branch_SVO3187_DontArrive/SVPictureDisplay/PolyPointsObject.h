//*****************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a polypoints object.
//*****************************************************************************
#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "SVPictureDisplayidl.h"
#pragma endregion Includes

class PolyPointsObject : public DrawObject
{
#pragma region Constructor
public:
	PolyPointsObject();
	virtual ~PolyPointsObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void Draw(POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected) override;
	virtual bool IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const override;
	virtual bool Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint) override;
	virtual void SetParameter(const VariantParamMap& ParameterMap) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//************************************
	// Method:    setPoints
	// Description: Set the points and recalculate the border rectangle. It delete the old points and set only the new points.
	// Parameter: _variant_t variantPoints Must be a array of long (VT_I4) or double (VT_R8) in the order x1,y1,x2,y2...
	// Returns:   void
	//************************************
	void setPoints(const _variant_t& variantPoints);

	//************************************
	// Method:    getPointsAsVariant
	// Description: Get the current points in a safeArray.
	// Returns:   _variant_t Return a safeArray of longs (VT_I4) in the order  x1,y1,x2,y2...
	//************************************
	_variant_t getPointsAsVariant() const;
#pragma endregion

#pragma region Member Variables
protected:
	CRect m_borderRect;
	std::vector<CPoint> m_points;
	ROISubType_PolyPoints m_subType; 
#pragma endregion Member Variables
};
