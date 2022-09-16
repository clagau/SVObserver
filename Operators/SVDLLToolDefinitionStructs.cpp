// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

InputValueDefinitionStruct& InputValueDefinitionStruct::operator = (const InputValueDefinitionStruct& rRhs)
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

ResultValueDefinitionStruct& ResultValueDefinitionStruct::operator = (const ResultValueDefinitionStruct& rRhs)
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

long InputValueDefinition::getVt() const
{
	return m_VT;
}

std::string InputValueDefinition::getDisplayName() const
{
	return m_DisplayName;
}

std::string InputValueDefinition::getHelpText() const
{
	
	return m_HelpText;

}

std::string InputValueDefinition::getGroup() const
{
	return  m_Group;
}


const _variant_t& InputValueDefinition::getDefaultValue() const
{

	return m_DefaultValue;

}

void InputValueDefinition::setDefinition(const InputValueDefinitionStruct&  InputValueDefStruct, long* pNLValue)
{
	m_Dim = 0;
	m_Type = SvPb::ExDllInterfaceType::Invalid;
	m_LinkedValueIndex = -1;
	m_VT = InputValueDefStruct.m_VT;
	m_DisplayName = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.m_bDisplayName));
	m_HelpText = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.m_bHelpText));
	m_Group = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.m_bGroup));
	m_DefaultValue = InputValueDefStruct.m_DefaultValue;

	m_UseDisplaynames = false;

	if (!(m_DefaultValue.vt & VT_ARRAY))
	{
		m_Type = SvPb::ExDllInterfaceType::Scalar;
		m_LinkedValueIndex = (*pNLValue)++;
	}
	else
	{
		m_Dim = SafeArrayGetDim(m_DefaultValue.parray);

		if (m_Dim == 1)
		{
			if (m_DefaultValue.vt == (VT_ARRAY | VT_BSTR))
			{
				//until now BSTR Arrays are only used for TablRowNames
				/// therefore the same index like the table before
				m_Type = SvPb::ExDllInterfaceType::TableNames;
				m_LinkedValueIndex = (*pNLValue) - 1;
			}
			else if ((m_DefaultValue.vt == (VT_ARRAY | VT_R8))
				|| (m_DefaultValue.vt == (VT_ARRAY | VT_I4)))
			{
				m_Type = SvPb::ExDllInterfaceType::Array;
				m_LinkedValueIndex = (*pNLValue)++;
			}
			else
			{
				throw std::invalid_argument("Invalid::DefaultValue dim == 1");
			}

		}
		else if (m_Dim == 2 && (m_DefaultValue.vt == (VT_ARRAY | VT_R8)))
		{
			m_Type = SvPb::ExDllInterfaceType::TableArray;
			m_LinkedValueIndex = (*pNLValue)++;
		}
		else
		{
			throw std::invalid_argument("Invalid::DefaultValue");
		}
	}
}


void InputValueDefinition::setDefinition(const InputValueDefinitionStructEx&  InputValueDefStruct, long* pNLValue)
{
	m_Dim = 0;
	m_Type = SvPb::ExDllInterfaceType::Invalid;
	m_LinkedValueIndex = -1;
	m_VT = InputValueDefStruct.vt;
	m_DisplayName = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.Name.copy()));
	m_HelpText = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.HelpText.copy()));
	m_Group = static_cast<LPSTR>(_bstr_t(InputValueDefStruct.Group.copy()));
	m_DefaultValue = InputValueDefStruct.vDefaultValue;

	m_UseDisplaynames = true;

	if (!(m_DefaultValue.vt & VT_ARRAY))
	{
		m_Type = SvPb::ExDllInterfaceType::Scalar;
		m_LinkedValueIndex = (*pNLValue)++;
	}
	else
	{
		m_Dim = SafeArrayGetDim(m_DefaultValue.parray);

		if (m_Dim == 1)
		{
			if (m_DefaultValue.vt == (VT_ARRAY | VT_BSTR))
			{
				//until now BSTR Arrays are only used for TablRowNames
				/// therefore the same index like the table before
				m_Type = SvPb::ExDllInterfaceType::TableNames;
				m_LinkedValueIndex = (*pNLValue) - 1;
			}
			else if ((m_DefaultValue.vt == (VT_ARRAY | VT_R8))
				|| (m_DefaultValue.vt == (VT_ARRAY | VT_I4)))
			{
				m_Type = SvPb::ExDllInterfaceType::Array;
				m_LinkedValueIndex = (*pNLValue)++;
			}
			else
			{
				throw std::invalid_argument("Invalid::DefaultValue dim == 1");
			}

		}
		else if (m_Dim == 2 && (m_DefaultValue.vt == (VT_ARRAY | VT_R8)))
		{
			m_Type = SvPb::ExDllInterfaceType::TableArray;
			m_LinkedValueIndex = (*pNLValue)++;
		}
		else
		{
			throw std::invalid_argument("Invalid::DefaultValue");
		}
	}
}


ResultTableDefinitionStruct& ResultTableDefinitionStruct::operator = (const ResultTableDefinitionStruct& rhs)
{
	if (this != &rhs)
	{
		lVT = rhs.lVT;
		bstrDisplayName = rhs.bstrDisplayName.copy();
		type = rhs.type;
		ColoumnCount = rhs.ColoumnCount;
		RowCount = rhs.RowCount;

		ColumnNames = rhs.ColumnNames;
	}
	return *this;
}
ResultTableDefinitionStruct::ResultTableDefinitionStruct(const ResultTableDefinitionStruct& rRhs) 
{
	*this = rRhs;
}

void ResultValueDefinition::setDefinition(const ResultValueDefinitionStruct&  resultValueDefinitionStruct, long ValueIndex)
{
	m_ValueIndex = ValueIndex;
	m_DisplayName = static_cast<LPCSTR>(_bstr_t(resultValueDefinitionStruct.m_bDisplayName));
	m_VT = 	resultValueDefinitionStruct.m_VT;

	m_UseDisplayNames = false;
}




void ResultValueDefinition::setDefinition(const ResultValueDefinitionStructEx&  resultValueDefinitionStruct, long ValueIndex)
{
	m_ValueIndex = ValueIndex;
	
	/*
	The operator LPSTR must not be called from resultValueDefinitionStruct.Displayname direct
	because the operator allocates memory and the destructor from resultValueDefinitionStruct
	is called in the external dll
	*/
	m_DisplayName = static_cast<LPCSTR>(_bstr_t( resultValueDefinitionStruct.Name.copy()));
	m_HelpText = static_cast<LPCSTR>(_bstr_t(resultValueDefinitionStruct.HelpText.copy()));
	m_Group = static_cast<LPCSTR>(_bstr_t(resultValueDefinitionStruct.Group.copy()));
	m_VT = resultValueDefinitionStruct.vt;
	m_MaxArraysize = resultValueDefinitionStruct.ArraySize;
	m_UseDisplayNames = true;
}

int ResultValueDefinition::getIndex() const
{
	return m_ValueIndex;
}

long ResultValueDefinition::getVT() const
{
	return m_VT;
}
std::string ResultValueDefinition::getDisplayName() const
{
	return m_DisplayName;
}
void ResultValueDefinition::setMaxArraysize(long size)
{
	if (size > 0)
	{
		m_MaxArraysize = size;
	}
}


long ResultValueDefinition::getMaxArraysize() const
{
	return m_MaxArraysize;
}


void ResultTableDefinition::setDefinition(const ResultTableDefinitionStruct&  DefinitionStruct, long Index)
{
	
	m_vt = DefinitionStruct.lVT;
	m_ColoumnCount = DefinitionStruct.ColoumnCount;
	m_RowCount = DefinitionStruct.RowCount;
	m_ColumnNames = DefinitionStruct.ColumnNames;
	m_UseDisplayNames = false;
	m_ValueIndex = Index;
	m_HelpText.clear();
	m_Group.clear();
	m_DisplayName = static_cast<LPCSTR>(_bstr_t(DefinitionStruct.bstrDisplayName.copy()));
	

	
	if (m_vt != VT_R8 ||
		(m_ColumnNames.vt != (VT_ARRAY | VT_BSTR)))
	{
		throw(std::invalid_argument("Wrong vt in ResultTableDefinition"));
	}
}
void ResultTableDefinition::setDefinition(const ResultTableDefinitionStructEx&  DefinitionStruct, long Index)
{
	m_vt = DefinitionStruct.vt;
	m_ColoumnCount = DefinitionStruct.ColoumnCount;
	m_RowCount = DefinitionStruct.RowCount;
	m_ColumnNames = DefinitionStruct.ColumnNames;
	m_UseDisplayNames = true;
	m_ValueIndex = Index;
	m_HelpText = static_cast<LPCSTR>(_bstr_t(DefinitionStruct.HelpText.copy()));
	m_Group = static_cast<LPCSTR>(_bstr_t(DefinitionStruct.Group.copy()));
	m_DisplayName = static_cast<LPCSTR>(_bstr_t(DefinitionStruct.Name.copy()));

	if (m_vt != VT_R8 ||
		(m_ColumnNames.vt != (VT_ARRAY | VT_BSTR)))
	{
		throw(std::invalid_argument("Wrong vt in ResultTableDefinition"));
	}
}


std::vector<std::string>  ResultTableDefinition::getColoumnNames() const
{
	std::vector<std::string> Ret;
	if (m_ColoumnCount > 0 && (m_ColumnNames.vt == (VT_ARRAY | VT_BSTR)))
	{
		CComSafeArray<BSTR> saInput(m_ColumnNames.parray);
		int dim = saInput.GetDimensions();
		assert(dim == 1);	UNREFERENCED_PARAMETER(dim);
		int len = saInput.GetCount();
		for (int y = 0; y < len; y++)
		{
			CComBSTR  name = saInput.GetAt(y);
			_bstr_t bstrname = name.m_str;
			Ret.push_back(static_cast<LPCSTR>(bstrname));
		}

	}
	return Ret;
}


std::string ResultTableDefinition::getDisplayName() const
{
	return m_DisplayName;
}

void InputImageInformationStruct::allowBlackAndWhite()
{
	AllowedImageTypes |= 1u; //currently this means 'basic' Black and white
}

void InputImageInformationStruct::allowColor()
{
	AllowedImageTypes |= 1llu<<32; //currently this means 'basic' Color
}

bool InputImageInformationStruct::mayBeBlackAndWhite() const
{
	return (AllowedImageTypes & 0xFFFFFFFF)!=0; //see definition of InputImageInformationStruct
}

bool InputImageInformationStruct::mayBeColor() const
{
	return (AllowedImageTypes & 0xFFFFFFFFllu<<32) != 0;  //see definition of InputImageInformationStruct
}

} //namespace SvOp
