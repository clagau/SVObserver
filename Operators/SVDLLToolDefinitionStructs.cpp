// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolDefinitionStructs
// * .File Name       : $Workfile:   SVDLLToolDefinitionStructs.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 10:18:22  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDLLToolDefinitionStructs.h"

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations





InputValueDefinitionStruct::InputValueDefinitionStruct() :
	m_VT(VT_EMPTY)
	, m_bDisplayName(nullptr)
	, m_bGroup(nullptr)
	, m_bHelpText(nullptr)
{
}

InputValueDefinitionStruct::InputValueDefinitionStruct(const InputValueDefinitionStruct& rRhs) :
	m_VT(VT_EMPTY)
	, m_bDisplayName(nullptr)
	, m_bGroup(nullptr)
	, m_bHelpText(nullptr)
{
	*this = rRhs;
}

InputValueDefinitionStruct::~InputValueDefinitionStruct()
{
	Clear();
}

const InputValueDefinitionStruct& InputValueDefinitionStruct::operator = (const InputValueDefinitionStruct& rRhs)
{

	if (this != &rRhs)
	{
		Clear();
		m_VT = rRhs.m_VT;
		m_DefaultValue = rRhs.m_DefaultValue;
		m_bDisplayName = _bstr_t(rRhs.m_bDisplayName).copy();
		m_bGroup = _bstr_t(rRhs.m_bGroup).copy();
		m_bHelpText = _bstr_t(rRhs.m_bHelpText).copy();
	}
	return *this;
}

void InputValueDefinitionStruct::Clear()
{
	m_VT = VT_EMPTY;
	m_DefaultValue.Clear();
	if (nullptr != m_bDisplayName)
	{
		::SysFreeString(m_bDisplayName);
		m_bDisplayName = nullptr;
	}
	if (nullptr != m_bGroup)
	{
		::SysFreeString(m_bGroup);
		m_bGroup = nullptr;
	}
	if (nullptr != m_bHelpText)
	{
		::SysFreeString(m_bHelpText);
		m_bHelpText = nullptr;
	}
}

bool InputValueDefinitionStruct::operator == (const InputValueDefinitionStruct& rRhs)
{
	bool bEqual = true;

	bEqual = bEqual && (m_VT == rRhs.m_VT);
	bEqual = bEqual && (_bstr_t(m_bDisplayName) == _bstr_t(rRhs.m_bDisplayName));
	bEqual = bEqual && (_bstr_t(m_bGroup) == _bstr_t(rRhs.m_bGroup));
	bEqual = bEqual && (_bstr_t(m_bHelpText) == _bstr_t(rRhs.m_bHelpText));
	bEqual = bEqual && (_bstr_t(m_DefaultValue) == _bstr_t(rRhs.m_DefaultValue));

	return bEqual;
}



// ResultValueDefinitionStruct
ResultValueDefinitionStruct::ResultValueDefinitionStruct() :
	m_VT(VT_EMPTY)
	, m_bDisplayName(nullptr)
{
}

ResultValueDefinitionStruct::ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rRhs) :
	m_VT(VT_EMPTY)
	, m_bDisplayName(nullptr)
{
	*this = rRhs;
}

ResultValueDefinitionStruct::~ResultValueDefinitionStruct()
{
	Clear();
}

const ResultValueDefinitionStruct& ResultValueDefinitionStruct::operator = (const ResultValueDefinitionStruct& rRhs)
{

	if (this != &rRhs)
	{
		Clear();
		m_VT = rRhs.m_VT;
		m_bDisplayName = _bstr_t(rRhs.m_bDisplayName).copy();
	}
	return *this;
}

void ResultValueDefinitionStruct::Clear()
{
	m_VT = VT_EMPTY;
	if (nullptr != m_bDisplayName)
	{
		::SysFreeString(m_bDisplayName);
		m_bDisplayName = nullptr;
	}
}

long InputValueDefinition::GetVt() const
{
	return m_InputValueDefStruct.m_VT;
}
std::string InputValueDefinition::getDisplayName() const
{
	_bstr_t ret(m_InputValueDefStruct.m_bDisplayName);
	std::string result((LPCSTR)ret);
	return result;
}

std::string InputValueDefinition::getHelpText() const
{
	_bstr_t ret(m_InputValueDefStruct.m_bHelpText);
	std::string result((LPCSTR)ret);
	return result;

}

std::string InputValueDefinition::getGroup() const
{
	_bstr_t ret(m_InputValueDefStruct.m_bGroup);
	std::string result((LPCSTR)ret);
	return result;
}


const _variant_t& InputValueDefinition::getDefaultValue() const
{

	return m_InputValueDefStruct.m_DefaultValue;

}
void InputValueDefinition::setDefinition(const InputValueDefinitionStruct&  InputValueDefStruct, long* pNLValue)
{
	m_Dim = 0;
	m_Type = SvOp::ExDllInterfaceType::Invalid;
	m_LinkedValueIndex = -1 ;
	m_InputValueDefStruct = InputValueDefStruct;

	if (!(m_InputValueDefStruct.m_DefaultValue.vt & VT_ARRAY))
	{
		m_Type = SvOp::ExDllInterfaceType::Scalar;
		m_LinkedValueIndex = (*pNLValue)++;
	}
	else
	{
		m_Dim = SafeArrayGetDim(m_InputValueDefStruct.m_DefaultValue.parray);

		if (m_Dim == 1)
		{
			if (m_InputValueDefStruct.m_DefaultValue.vt & VT_BSTR)
			{
				//until now BSTR Arrays are only used for TablRowNames
				/// therefore the same index like the table before
				m_Type = SvOp::ExDllInterfaceType::TableNames;
				m_LinkedValueIndex = (*pNLValue)-1;
			}
			else if ((m_InputValueDefStruct.m_DefaultValue.vt &  VT_R8)
				|| (m_InputValueDefStruct.m_DefaultValue.vt &  VT_I4))
			{
				m_Type = SvOp::ExDllInterfaceType::Array;
				m_LinkedValueIndex = (*pNLValue)++;
			}

		}
		else if (m_Dim == 2 && (m_InputValueDefStruct.m_DefaultValue.vt == (VT_ARRAY | VT_R8)))
		{
			m_Type = SvOp::ExDllInterfaceType::TableArray;
			m_LinkedValueIndex = (*pNLValue)++;
		}
	}
}



} //namespace SvOp
