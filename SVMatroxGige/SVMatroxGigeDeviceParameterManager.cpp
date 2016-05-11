//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDeviceParameterManager
//* .File Name       : $Workfile:   SVMatroxGigeDeviceParameterManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:42:16  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVMatroxGigeDeviceParameterManager.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVGigeStandardCameraFeatures.h"
#include "SVGigeCameraDatabase.h"

const SVGigeDeviceParameterMap& SVMatroxGigeDeviceParameterManager::GetParameterMap()
{
	// Assign Standard features
	return SVGigeStandardCameraFeatures::GetStandardFeatures();
}

HRESULT SVMatroxGigeDeviceParameterManager::GetParameterName(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, BSTR* p_pBstrName)
{
	HRESULT l_hr = S_FALSE;
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SVGigeParameterEnum>(p_iParameterID));
	if (it != gigeParameters.end())
	{
		*p_pBstrName = _bstr_t(it->second.name.c_str()).Detach();
		l_hr = S_OK;
	}
	else // could be custom - look in overrides
	{
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		if (!gigeParam.name.empty())
		{
			*p_pBstrName = _bstr_t(gigeParam.name.c_str()).Detach();
			l_hr = S_OK;
		}
	}
	return l_hr;
}

HRESULT SVMatroxGigeDeviceParameterManager::GetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue)
{
	HRESULT l_hr = S_FALSE;
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SVGigeParameterEnum>(p_iParameterID));
	if (it != gigeParameters.end())
	{
		// what should go in p_piParameterTypeID ???
		//*p_piParameterTypeID = it->second.dataType;

		_variant_t value(0);
		
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		const SVString& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			value.ChangeType(gigeParam.dataType);

			l_hr = gigeParam.accessor.GetParam(p_rCamera.m_Digitizer, gigeParam.accessor.feature, value);
		}
		else
		{
			// do the type conversion
			value.ChangeType(it->second.dataType);

			l_hr = it->second.accessor.GetParam(p_rCamera.m_Digitizer, it->second.accessor.feature, value);
		}
		//if (S_OK == l_hr)
		{
			*p_pvarValue = value.Detach();
		}
	}
	else // could be custom - look in overrides
	{
		_variant_t value(0);
		
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		const SVString& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			value.ChangeType(gigeParam.dataType);

			l_hr = gigeParam.accessor.GetParam(p_rCamera.m_Digitizer, gigeParam.accessor.feature, value);

			*p_pvarValue = value.Detach();
		}
	}
	return l_hr;
}

HRESULT SVMatroxGigeDeviceParameterManager::SetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue)
{
	HRESULT l_hr = S_FALSE;

	_variant_t value = *p_pvarValue;

	// what types are in p_iParameterTypeID ???
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SVGigeParameterEnum>(p_iParameterID));
	if (it != gigeParameters.end())
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		const SVString& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			value.ChangeType(gigeParam.dataType);

			// Set the Feature value
			l_hr = gigeParam.accessor.SetParam(p_rCamera.m_Digitizer, gigeParam.accessor.feature, value);
		}
		else
		{
			// do the type conversion
			value.ChangeType(it->second.dataType);

			// Set the Feature value
			l_hr = it->second.accessor.SetParam(p_rCamera.m_Digitizer, it->second.accessor.feature, value);
		}
	}
	else // could be custom - look in overrides
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		const SVString& featureName = gigeParam.accessor.feature.GetName();
		if (!featureName.empty())
		{
			// do the type conversion
			value.ChangeType(gigeParam.dataType);

			// Set the Feature value
			l_hr = gigeParam.accessor.SetParam(p_rCamera.m_Digitizer, gigeParam.accessor.feature, value);
		}
	}
	return l_hr;
}

HRESULT SVMatroxGigeDeviceParameterManager::IsParameterSupported(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID)
{
	HRESULT l_hr = S_FALSE;
	
	const SVGigeDeviceParameterMap& gigeParameters = SVMatroxGigeDeviceParameterManager::GetParameterMap();
	SVGigeDeviceParameterMap::const_iterator it = gigeParameters.find(static_cast<SVGigeParameterEnum>(p_iParameterID));
	if (it != gigeParameters.end())
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		if( gigeParam.accessor.feature.IsSupported() )
		{
			l_hr = S_OK;
		}
	}
	else // could be custom - look in overrides
	{
		// Look for any exception in the Gige Feature Overrides
		const SVGigeDeviceParameterStruct& gigeParam = p_rCamera.GetFeature(static_cast<SVGigeParameterEnum>(p_iParameterID));
		if( gigeParam.accessor.feature.IsSupported() )
		{
			l_hr = S_OK;
		}
	}
	return l_hr;
}

