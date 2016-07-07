//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeParameterAccessor
//* .File Name       : $Workfile:   SVGigeParameterAccessor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:57:04  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVGigeParameterAccessor.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"
#pragma endregion Includes

// This Method uses the default Setter and getter
SVGigeParameterAccessor::SVGigeParameterAccessor(const SVGigeFeature& rFeature)
: feature(rFeature)
, GetParam(boost::bind(SVGigeParameterAccessor::GetFeature, _1, _2, _3))
, SetParam(boost::bind(SVGigeParameterAccessor::SetFeature, _1, _2, _3))
{
}

// This Method is for overriding Setter and getter that do not require the Feature
SVGigeParameterAccessor::SVGigeParameterAccessor(const SVGigeFeature& p_Feature, const ParamGetter& getterFunc, const ParamSetter& setterFunc)
: feature(p_Feature)
, GetParam(getterFunc)
, SetParam(setterFunc)
{
}

SVGigeParameterAccessor::SVGigeParameterAccessor(const SVGigeParameterAccessor& rAccessor)
: feature(rAccessor.feature)
, GetParam(rAccessor.GetParam)
, SetParam(rAccessor.SetParam)
{
}

SVGigeParameterAccessor& SVGigeParameterAccessor::operator=(const SVGigeParameterAccessor& rAccessor)
{
	if (this != &rAccessor)
	{
		feature = rAccessor.feature;
		GetParam = rAccessor.GetParam;
		SetParam = rAccessor.SetParam;
	}
	return *this;
}

// This Method is just a call through for getting the feature
HRESULT SVGigeParameterAccessor::GetFeature(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue)
{
	HRESULT hr = S_OK;
	if (rFeature.IsSupported())
	{
		// Check if have selector
		if (rFeature.HasSelector())
		{
			const SVGigeFeatureSelector& selector = rFeature.GetSelector();
			hr = SVMatroxDigitizerInterface::SetFeature( *(Digitizer.get()), SVString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
		}
		// Translate Strings
		if (rFeature.HasTranslation())
		{
			variant_t value("");
			hr = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVString(rFeature.GetName().c_str()), rFeature.GetType(), value);
			if (S_OK == hr)
			{
				SVGigeFeatureString gigeFeatureValue(SvUl_SF::createSVString(value));
				SVDeviceParamString deviceParamString;
		
				hr = rFeature.GetDeviceParamString(gigeFeatureValue, deviceParamString);
				rValue = deviceParamString.c_str();
			}
		}
		else
		{
			hr = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		}
	}
	return hr;
}

// This Method is just a call through for setting the feature
HRESULT SVGigeParameterAccessor::SetFeature(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue)
{
	HRESULT hr = S_OK;
	if (rFeature.IsSupported() && !rFeature.IsReadOnly())
	{
		// Check if have selector
		if (rFeature.HasSelector())
		{
			const SVGigeFeatureSelector& selector = rFeature.GetSelector();
			hr = SVMatroxDigitizerInterface::SetFeature( *(Digitizer.get()), SVString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
		}

		// Translate Strings
		if (rFeature.HasTranslation())
		{
			SVDeviceParamString deviceParamString(SvUl_SF::createSVString(rValue));
			SVGigeFeatureString gigeFeatureValue;
			
			hr = rFeature.GetGigeFeatureString(deviceParamString, gigeFeatureValue);
			if (S_OK == hr)
			{
				variant_t value(gigeFeatureValue.c_str());
				hr = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVString(rFeature.GetName().c_str()), rFeature.GetType(), value);
			}
		}
		else
		{
			hr = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		}
	}
	return hr;
}

