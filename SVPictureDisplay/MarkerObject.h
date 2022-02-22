//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MarkerObject
//* .File Name       : $Workfile:   MarkerObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "DrawObject.h"
#include "SVPictureDisplayidl.h"
#pragma endregion Includes

class MarkerObject : public DrawObject
{
public:
#pragma region Constructor
	MarkerObject();
	virtual ~MarkerObject();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual bool IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const override;
	virtual bool Move( HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint ) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
	virtual RECT GetRectangle() const override;
#pragma endregion Public Methods

#pragma region Private Methods
	double GetValuePositionInImage(int imageSize) const;
	double GetValueFromPositionInImage(double position, int imageSize) const;
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	ROISubType_Marker m_subtype;
	double m_min_Value;
	double m_max_Value;
	double m_Value;
#pragma endregion Member Variables
};

