//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedValue
//* .File Name       : $Workfile:   SVSharedValue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:08  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVShared.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	// shared memory structure
	struct SVSharedValue
	{
		enum ResultTypeEnum
		{
			UnknownType = 0,
			StringType = 1,
			IntType = 2,
			FloatType = 3,
			BooleanType = 4
		};
		
		SVSharedValue(const void_allocator& rAlloc);
		SVSharedValue(const SVSharedValue& rData);
		const SVSharedValue& operator=(const SVSharedValue& rData);
		void SetName(const SVString& Name);
		void SetData(ResultTypeEnum ResultType, const SVString& Result, int Status);


		bip_string m_ElementName;
		ResultTypeEnum m_ResultType;	// this is the native type
		char m_Result[statics::max_result_size];
		int m_Status;
		void_allocator m_Allocator;

	};

	// local memory structure
	struct SVValue
	{
		long trigger;
		long status;
		SVString name;
		SVString value;

		SVValue(const SVString& n, long trg, long sts, const SVString& val)
		: name(n), trigger(trg), status(sts), value(val)
		{
		}
		SVValue() {}

		bool empty() const { return name.empty(); }
	};
	typedef bip::allocator< SVSharedValue, segment_manager_t > SVSharedValueAllocator;
	typedef bip::vector< SVSharedValue, SVSharedValueAllocator > SVSharedValueVector;
	typedef bip::allocator< SVSharedValueVector, segment_manager_t > SVSharedValueVectorAllocator;

	void  CopySharedValues( SVSharedValueVector& rTo  , const SVSharedValueVector& rfrom);
} //namespace SvSml
