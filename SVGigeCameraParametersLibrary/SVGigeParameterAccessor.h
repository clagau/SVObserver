//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeParameterAccessor
//* .File Name       : $Workfile:   SVGigeParameterAccessor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:57:06  $
//******************************************************************************
#pragma once
#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVGigeFeature.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#pragma endregion Includes

struct SVGigeParameterAccessor
{
	typedef boost::function<HRESULT(SVMatroxDigitizerRef, const SVGigeFeature& rFeature, _variant_t&)> ParamGetter;
	typedef boost::function<HRESULT(SVMatroxDigitizerRef, const SVGigeFeature& rFeature, const _variant_t&)> ParamSetter;

	ParamGetter GetParam;
	ParamSetter SetParam;

	SVGigeFeature feature;

	// This Method uses the default Setter and getter
	SVGigeParameterAccessor(const SVGigeFeature& rFeature);

	// This Method is for overriding Setter and Getter
	SVGigeParameterAccessor(const SVGigeFeature& rFeature, const ParamGetter& getterFunc, const ParamSetter& setterFunc);
	
	SVGigeParameterAccessor(const SVGigeParameterAccessor& rAccessor);
	SVGigeParameterAccessor& operator=(const SVGigeParameterAccessor& rAccessor);

	// default Getter/Setter
	static HRESULT GetFeature(SVMatroxDigitizerRef digitizer, const SVGigeFeature& feature, _variant_t& value);
	static HRESULT SetFeature(SVMatroxDigitizerRef digitizer, const SVGigeFeature& feature, const _variant_t& value);
};

