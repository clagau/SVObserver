//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVIOEntryStruct.h
/// All Rights Reserved
//*****************************************************************************
/// The IO Entry structure used to define inputs and outputs
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVIOEntryHostStruct.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

struct SVIOEntryStruct
{
	SVIOEntryStruct() = default;
	SVIOEntryStruct(const SVIOEntryStruct& rRhs) = default;
	SVIOEntryStruct& operator=(SVIOEntryStruct&) = default;

	virtual ~SVIOEntryStruct();

	bool operator<(const SVIOEntryStruct& rRhs) const;

	bool empty() const;
	void clear();

	SVIOEntryHostStructPtr m_IOEntryPtr;

	bool m_EntryValid = false;
	bool m_CombinedValue = false;

};

typedef std::vector<SVIOEntryStruct> SVIOEntryStructVector;
