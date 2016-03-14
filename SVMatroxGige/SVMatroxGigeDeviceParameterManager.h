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
#ifndef SVMATROXGIGEDEVICEPARAMETERMANAGER_H
#define SVMATROXGIGEDEVICEPARAMETERMANAGER_H

#include <map>
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"

class SVMatroxGigeDigitizer;

class SVMatroxGigeDeviceParameterManager
{
private:
	static const SVGigeDeviceParameterMap& GetParameterMap();

public:
	static HRESULT GetParameterName(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, BSTR* p_pBstrName);
	static HRESULT GetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue);
	static HRESULT SetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int p_piParameterTypeID, VARIANT *p_pvarValue);
	static HRESULT IsParameterSupported(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID);
};

#endif

