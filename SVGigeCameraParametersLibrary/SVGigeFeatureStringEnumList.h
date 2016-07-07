//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureStringEnum
//* .File Name       : $Workfile:   SVGigeFeatureStringEnumList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "SVContainerLibrary/SVBidirectionalMap.h"
#pragma endregion Includes

typedef SVString SVDeviceParamString;
typedef SVString SVGigeFeatureString;

class SVGigeFeatureStringEnumList
{
	typedef SVBidirectionalMap<SVDeviceParamString, SVGigeFeatureString>::type GigeFeatureStringEnumList;
	GigeFeatureStringEnumList m_stringEnums;

public:
	SVGigeFeatureStringEnumList();
	~SVGigeFeatureStringEnumList();

	template<class _Iter>
	SVGigeFeatureStringEnumList(_Iter first, _Iter last);

	SVGigeFeatureStringEnumList(const SVGigeFeatureStringEnumList& rList);
	SVGigeFeatureStringEnumList& operator=(const SVGigeFeatureStringEnumList& rList);

	bool HasTranslation() const;
	HRESULT GetGigeFeatureString(const SVDeviceParamString& in, SVGigeFeatureString& out) const;
	HRESULT GetDeviceParamString(const SVGigeFeatureString& in, SVDeviceParamString& out) const;
};

#include "SVGigeFeatureStringEnumList.inl"

