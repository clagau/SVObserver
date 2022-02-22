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

template<typename SVEnumType>
HRESULT ConvertEnumToMatroxType(const std::vector<std::pair<SVEnumType, long long>>& rEnumPairList, SVEnumType Type, long long& rMatroxType)
{
	for(const auto& rEntry : rEnumPairList)
	{
		if(rEntry.first == Type)
		{
			rMatroxType = rEntry.second;
			return S_OK;
		}
	}
	return SVMEE_INTERNAL_CONVERSION_ERROR;
}

template<typename SVEnumType>
HRESULT ConvertEnumFromMatroxType(const std::vector<std::pair<SVEnumType, long long>>& rEnumPairList, long long MatroxType, SVEnumType& rType)
{
	for (const auto& rEntry : rEnumPairList)
	{
		if (rEntry.second == MatroxType)
		{
			rType = rEntry.first;
			return S_OK;
		}
	}
	return SVMEE_INTERNAL_CONVERSION_ERROR;
}

