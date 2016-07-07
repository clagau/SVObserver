//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDBParser
//* .File Name       : $Workfile:   SVGigeCameraDBParser.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:22  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVGigeDeviceParameterStruct.h"
#pragma endregion Includes

class SVGigeCameraDBParser
{
public:
	static HRESULT Parse(BSTR data, SVGigeDeviceParameterMap& params);

private:
	// Not constructable
	SVGigeCameraDBParser();
	~SVGigeCameraDBParser();
};

