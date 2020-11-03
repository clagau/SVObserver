//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDeviceParameterManager
//* .File Name       : $Workfile:   SVMatroxGigeDeviceParameterManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:16  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#pragma endregion Includes

class SVMatroxGigeDigitizer;

class SVMatroxGigeDeviceParameterManager
{
private:
	static const SVGigeDeviceParameterMap& GetParameterMap();

public:
	static _variant_t GetParameterName(const SVMatroxGigeDigitizer& rCamera, int parameterID);
	static _variant_t GetParameter(const SVMatroxGigeDigitizer& rCamera, int parameterID);
	static HRESULT SetParameter(const SVMatroxGigeDigitizer& rCamera, int parameterID, const _variant_t& rValue);
	static HRESULT IsParameterSupported(const SVMatroxGigeDigitizer& rCamera, int parameterID);
};

