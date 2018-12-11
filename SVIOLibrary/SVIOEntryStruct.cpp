//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVIOEntryStruct.cpp
/// All Rights Reserved
//*****************************************************************************
/// The IO Entry structure used to define inputs and outputs
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIOEntryStruct.h"
#pragma endregion Includes

SVIOEntryStruct::SVIOEntryStruct()
	: m_IOEntryPtr()
	, m_EntryValid(false)
	, m_CombinedValue(false)
{
}

SVIOEntryStruct::SVIOEntryStruct(const SVIOEntryStruct& rRhs)
	: m_IOEntryPtr(rRhs.m_IOEntryPtr)
	, m_EntryValid(rRhs.m_EntryValid)
	, m_CombinedValue(rRhs.m_CombinedValue)
{
}

SVIOEntryStruct::~SVIOEntryStruct()
{
	clear();
}

bool SVIOEntryStruct::operator<(const SVIOEntryStruct& rRhs) const
{
	return (m_IOEntryPtr < rRhs.m_IOEntryPtr);
}

bool SVIOEntryStruct::empty() const
{
	return (nullptr == m_IOEntryPtr);
}

void SVIOEntryStruct::clear()
{
	m_IOEntryPtr.reset();

	m_EntryValid = false;
	m_CombinedValue = false;
}

void SVIOEntryStruct::InitEntry()
{
	m_EntryValid = false;
	m_CombinedValue = false;
}
