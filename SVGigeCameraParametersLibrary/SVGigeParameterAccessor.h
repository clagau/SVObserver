//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVGigeFeature.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#pragma endregion Includes

struct SVGigeParameterAccessor
{
	using ParamGetter = std::function<HRESULT(SVMatroxDigitizerPtr, const SVGigeFeature& rFeature, _variant_t&)>;
	using ParamSetter = std::function<HRESULT(SVMatroxDigitizerPtr, const SVGigeFeature& rFeature, const _variant_t&)>;

	ParamGetter GetParam;
	ParamSetter SetParam;

	SVGigeFeature feature;

	// This Method uses the default Setter and getter
	explicit SVGigeParameterAccessor(const SVGigeFeature& rFeature);

	// This Method is for overriding Setter and Getter
	SVGigeParameterAccessor(const SVGigeFeature& rFeature, const ParamGetter& getterFunc, const ParamSetter& setterFunc);
	
	SVGigeParameterAccessor(const SVGigeParameterAccessor& rAccessor);
	SVGigeParameterAccessor& operator=(const SVGigeParameterAccessor& rAccessor);

	// default Getter/Setter
	static HRESULT GetFeature(SVMatroxDigitizerPtr digitizer, const SVGigeFeature& feature, _variant_t& value);
	static HRESULT SetFeature(SVMatroxDigitizerPtr digitizer, const SVGigeFeature& feature, const _variant_t& value);
};

