//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ArrowObject
//* .File Name       : $Workfile:   ArrowObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:36  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "LineObject.h"
#include "SVPictureDisplayidl.h"
#pragma endregion Includes

class ArrowObject : public LineObject
{
	enum
	{
		SVArrowPixels = 10,
	};

public:
#pragma region Constructor
	ArrowObject();
	virtual ~ArrowObject();
#pragma endregion Constructor

#pragma region Public Methods
	virtual void Draw( POINT p_dOffset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected ) override;
	virtual void SetParameter(long parameterId, _variant_t parameterValue) override;
	virtual void GetParameter(VariantParamMap& ParameterMap) const override;
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SVOrientation m_eOrient {None};
#pragma endregion Member Variables
};

