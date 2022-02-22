//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDeviceParameterManager
//* .File Name       : $Workfile:   SVMatroxGigeDeviceParameterManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:42:16  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVMatroxGigeDeviceParameterManager.h"

#include "SVGigeStandardCameraFeatures.h"
#pragma endregion Includes

const SVGigeDeviceParameterMap& SVMatroxGigeDeviceParameterManager::GetParameterMap()
{
	// Assign Standard features
	return SVGigeStandardCameraFeatures::GetStandardFeatures();
}

_variant_t SVMatroxGigeDeviceParameterManager::GetParameterName(const SVMatroxGigeDigitizer& rCamera, int parameterID)
{
	_variant_t result;

	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
	if (it != gigeParameters.end())
	{
		result.SetString(it->second.name.c_str());
	}
	else // could be custom - look in overrides
	{
		const SVGigeDeviceParameterStruct& gigeParam = rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
		if (!gigeParam.name.empty())
		{
			result.SetString(gigeParam.name.c_str());
		}
	}
	
	return result;
}

_variant_t SVMatroxGigeDeviceParameterManager::GetParameter(const SVMatroxGigeDigitizer& rCamera, int parameterID)
{
	_variant_t result;
	
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
	if (it != gigeParameters.end())
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
		const std::string& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			result.ChangeType(gigeParam.dataType);
			gigeParam.accessor.GetParam(rCamera.m_Digitizer, gigeParam.accessor.feature, result);
		}
		else
		{
			result.ChangeType(it->second.dataType);
			it->second.accessor.GetParam(rCamera.m_Digitizer, it->second.accessor.feature, result);
		}
	}
	else // could be custom - look in overrides
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
		const std::string& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			result.ChangeType(gigeParam.dataType);

			gigeParam.accessor.GetParam(rCamera.m_Digitizer, gigeParam.accessor.feature, result);
		}
	}
	return result;
}

HRESULT SVMatroxGigeDeviceParameterManager::SetParameter(const SVMatroxGigeDigitizer& rCamera, int parameterID, const _variant_t& rValue)
{
	HRESULT l_hr = S_FALSE;

	_variant_t setValue{ rValue };
	// what types are in p_iParameterTypeID ???
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
	if (it != gigeParameters.end())
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
		const std::string& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			setValue.ChangeType(gigeParam.dataType);

			// Set the Feature value
			l_hr = gigeParam.accessor.SetParam(rCamera.m_Digitizer, gigeParam.accessor.feature, setValue);
		}
		else
		{
			// do the type conversion
			setValue.ChangeType(it->second.dataType);

			// Set the Feature value
			l_hr = it->second.accessor.SetParam(rCamera.m_Digitizer, it->second.accessor.feature, setValue);
		}
	}
	else // could be custom - look in overrides
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(parameterID));
		const std::string& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			setValue.ChangeType(gigeParam.dataType);

			// Set the Feature value
			l_hr = gigeParam.accessor.SetParam(rCamera.m_Digitizer, gigeParam.accessor.feature, setValue);
		}
	}
	return l_hr;
}

HRESULT SVMatroxGigeDeviceParameterManager::IsParameterSupported(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID)
{
	HRESULT l_hr = S_FALSE;
	
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SvDef::SVGigeParameterEnum>(p_iParameterID));
	if (it != gigeParameters.end())
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(p_iParameterID));
		if( gigeParam.accessor.feature.IsSupported() )
		{
			l_hr = S_OK;
		}
	}
	else // could be custom - look in overrides
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SvDef::SVGigeParameterEnum>(p_iParameterID));
		if( gigeParam.accessor.feature.IsSupported() )
		{
			l_hr = S_OK;
		}
	}
	return l_hr;
}

