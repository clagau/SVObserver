//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxEnumConvertor
//* .File Name       : $Workfile:   SVMatroxEnumConvertor.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:13:06  $
//******************************************************************************

#include "SVMatroxErrorEnum.h"

template<typename PairContainer, typename SVEnumType>
HRESULT ConvertEnumToMatroxType(const PairContainer& rEnumPairList, SVEnumType Type, long long& rMatroxType)
{
	const auto iter = std::find_if(rEnumPairList.begin(), rEnumPairList.end(), [&Type](const auto& rEntry) {return rEntry.first == Type; });
	if (rEnumPairList.end() != iter)
	{
		rMatroxType = iter->second;
		return S_OK;
	}
	return SVMEE_INTERNAL_CONVERSION_ERROR;
}

template<typename PairContainer, typename SVEnumType>
HRESULT ConvertEnumFromMatroxType(const PairContainer& rEnumPairList, long long MatroxType, SVEnumType& rType)
{
	const auto iter = std::find_if(rEnumPairList.begin(), rEnumPairList.end(), [&MatroxType](const auto& rEntry) {return rEntry.second == MatroxType; });
	if (rEnumPairList.end() != iter)
	{
		rType = iter->first;
		return S_OK;
	}
	return SVMEE_INTERNAL_CONVERSION_ERROR;
}

