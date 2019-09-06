//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file CopyData.h
/// All Rights Reserved 
//*****************************************************************************
/// Function to copy the data
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf\ConverterHelper.h"
#include "SVMessage\SVMessage.h"
#include "ResetLocker.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvTrc
{
/// Copies the data list to the specified memory
/// \param ValueObjectList [in] Value object list (Move semantics!)
/// \param pDestination [in] pointer to the memory destination (if nullptr return required size)
/// \param maxByteSize [in] The maximum size of memory that can be copied
/// \param resetID [in] The record reset ID
inline int copyDataList(std::vector<_variant_t>&& valueObjectList, void* pDestination = nullptr, int maxByteSize = 0, long resetID = 0)
{
	int result {0};

	SvPb::DataList valueList;

	auto* pList = valueList.mutable_valuelist();
	for (const auto& rValue : valueObjectList)
	{
		auto pValue = pList->Add();
		SvPb::ConvertVariantToProtobuf(rValue, pValue);
	}
	result = valueList.ByteSize();

	if (nullptr != pDestination && 0 < valueList.valuelist_size())
	{
		auto pLock = ResetLocker::lockReset(resetID);
		if (nullptr != pLock)
		{
			//The destination start is the data valid flag
			std::atomic_int* pDataSize = reinterpret_cast<std::atomic_int*> (pDestination);
			//The next position is where the value data list is streamed
			void* pData = reinterpret_cast<void*> (pDataSize + 1);
			if (result <= maxByteSize)
			{
				valueList.SerializeToArray(pData, result);
				*pDataSize = result;
			}
			else
			{
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		else
		{
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	valueList.Clear();

	return result;
}

} // namespace SvTrc
