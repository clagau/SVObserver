//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportedInputList
//* .File Name       : $Workfile:   SVImportedInputList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 20:06:08  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <deque>

#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

typedef std::deque<boost::any> SVImportedInputList;

struct SVImportedInput
{
	const SVString name;
	const long ppqPosition;
	
	SVImportedInput(const SVString& rName, long ppqPos)
	: name(rName), ppqPosition(ppqPos) {}
	virtual ~SVImportedInput() {}
};

struct SVImportedRemoteInput : SVImportedInput
{
	const _variant_t value;
	const long index;
	SVImportedRemoteInput(const SVString& rName, long ppqPos, const _variant_t& rValue, long idx)
	: SVImportedInput(rName, ppqPos)
	, index(idx)
	, value(rValue)
	{
	}
};

