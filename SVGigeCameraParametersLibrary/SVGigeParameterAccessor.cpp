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
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVGigeParameterAccessor.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerFeatureEnums.h"

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
			hr = SVMatroxDigitizerInterface::SetFeature( *(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
		}
		// Translate Strings
		if (rFeature.HasTranslation())
		{
			variant_t value("");
			hr = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), value);
			if (hr == S_OK)
			{
				SVGigeFeatureString gigeFeatureValue(SvUl_SF::createSVString(value));
				SVDeviceParamString deviceParamString;
		
				hr = rFeature.GetDeviceParamString(gigeFeatureValue, deviceParamString);
				rValue = deviceParamString.c_str();
			}
		}
		else
		{
			hr = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
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
			hr = SVMatroxDigitizerInterface::SetFeature( *(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
		}

		// Translate Strings
		if (rFeature.HasTranslation())
		{
			SVDeviceParamString deviceParamString(SvUl_SF::createSVString(rValue));
			SVGigeFeatureString gigeFeatureValue;
			
			hr = rFeature.GetGigeFeatureString(deviceParamString, gigeFeatureValue);
			if (hr == S_OK)
			{
				variant_t value(gigeFeatureValue.c_str());
				hr = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), value);
			}
		}
		else
		{
			hr = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), rValue);
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeParameterAccessor.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:57:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Aug 2010 10:56:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated set parameter method to remove error return code when parameter is readonly or not supported.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2010 13:47:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:32:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:06:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
