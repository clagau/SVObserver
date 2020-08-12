//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureStringEnumList
//* .File Name       : $Workfile:   SVGigeFeatureStringEnumList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:58  $
//******************************************************************************
//*****************************************************************************
/// This class is a string mapping between Gige Feature and Device parameter
/// and vice verse (The Gige Feature or Device parameter can be given as the input 
/// and it returns the corresponding parameter)
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes


class SVGigeFeatureStringEnumList
{
	typedef std::vector<std::pair<std::string, std::string>> StringPairVector;
	StringPairVector m_stringEnums;

public:
	SVGigeFeatureStringEnumList();
	~SVGigeFeatureStringEnumList();

	template<class _Iter>
	SVGigeFeatureStringEnumList(_Iter first, _Iter last);

	SVGigeFeatureStringEnumList(const SVGigeFeatureStringEnumList& );
	//This is the initializer list constructor
	explicit SVGigeFeatureStringEnumList(const StringPairVector& rRhs);
	SVGigeFeatureStringEnumList& operator=(const SVGigeFeatureStringEnumList& rList);

	bool HasTranslation() const;

	//! Gets the Gige Feature string
	//! \param rDeviceParam [in] reference to the device parameter string
	//! \param rGigeFeature [out] reference to the corresponding Gige Feature string
	//! \returns S_OK on success
	HRESULT GetGigeFeatureString(const std::string& rDeviceParam, std::string& rGigeFeature) const;

	//! Gets the Device Parameter string
	//! \param rGigeFeature [in] reference to the Gige feature string
	//! \param rDeviceParam [out] reference to the corresponding Device Parameter string
	//! \returns S_OK on success
	HRESULT GetDeviceParamString(const std::string& rGigeFeature, std::string& rDeviceParam) const;
};

#include "SVGigeFeatureStringEnumList.inl"

