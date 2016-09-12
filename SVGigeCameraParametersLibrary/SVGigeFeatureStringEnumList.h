//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringEnum
//* .File Name       : $Workfile:   SVStringEnumList.h  $
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
#include "SVUtilityLibrary/SVString.h"
#include "SVContainerLibrary/SVBidirectionalMap.h"
#pragma endregion Includes


class SVGigeFeatureStringEnumList
{
public:
	SVBidirectionalMap<SVString, SVString>::type m_stringEnums;

	SVGigeFeatureStringEnumList();
	~SVGigeFeatureStringEnumList();

	template<class _Iter>
	SVGigeFeatureStringEnumList(_Iter first, _Iter last);

	SVGigeFeatureStringEnumList(const SVGigeFeatureStringEnumList& rList);
	SVGigeFeatureStringEnumList& operator=(const SVGigeFeatureStringEnumList& rList);

	bool HasTranslation() const;

	//! Gets the Gige Feature string
	//! \param rDeviceParam [in] reference to the device parameter string
	//! \param rGigeFeature [out] reference to the corresponding Gige Feature string
	//! \returns S_OK on success
	HRESULT GetGigeFeatureString(const SVString& rDeviceParam, SVString& rGigeFeature) const;

	//! Gets the Device Parameter string
	//! \param rGigeFeature [in] reference to the Gige feature string
	//! \param rDeviceParam [out] reference to the corresponding Device Parameter string
	//! \returns S_OK on success
	HRESULT GetDeviceParamString(const SVString& rGigeFeature, SVString& rDeviceParam) const;
};

#include "SVGigeFeatureStringEnumList.inl"

