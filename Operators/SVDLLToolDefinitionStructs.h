// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolDefinitionStructs
// * .File Name       : $Workfile:   SVDLLToolDefinitionStructs.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 10:18:32  $
// ******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes


namespace SvOp
{

enum class ExDllInterfaceType
{
	Scalar = 0, Array, TableArray, TableNames, Invalid
};

#pragma pack (push, 1)
struct InputValueDefinitionStruct
{
	long m_VT;
	BSTR m_bDisplayName;
	BSTR m_bHelpText;
	BSTR m_bGroup;
	_variant_t m_DefaultValue;
public:
	InputValueDefinitionStruct();
	~InputValueDefinitionStruct();
	InputValueDefinitionStruct(const InputValueDefinitionStruct& rhs);
	const InputValueDefinitionStruct& operator = (const InputValueDefinitionStruct& rhs);
	bool operator == (const InputValueDefinitionStruct& rhs);
private:
	void Clear();
};
#pragma pack (pop)



//!class describing the inputvalue definitions for External dll
// contents the structure from the dll with some additional calculated information 
class  InputValueDefinition
{

public:
	InputValueDefinition() = default;
	~InputValueDefinition() = default;

	long GetVt() const;
	std::string getDisplayName() const;
	std::string getHelpText() const;
	std::string getGroup() const;
	const _variant_t& getDefaultValue() const;
	void setDefinition(const InputValueDefinitionStruct&  InputValueDefStruct, long* NofLinkedValue);

	int getDim()const { return m_Dim; };
	SvOp::ExDllInterfaceType getType() const { return m_Type; };
	int  getLinkedValueIndex() const { return m_LinkedValueIndex; };

private:

	int m_LinkedValueIndex {-1};
	InputValueDefinitionStruct m_InputValueDefStruct;
	int							m_Dim {0}; //dimension of Arrays
	SvOp::ExDllInterfaceType             m_Type {SvOp::ExDllInterfaceType::Scalar};
};




#pragma pack (push, 1)
struct ResultValueDefinitionStruct
{
	long m_VT;
	BSTR m_bDisplayName;	// not used at this time
public:
	ResultValueDefinitionStruct();
	~ResultValueDefinitionStruct();
	ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rhs);
	const ResultValueDefinitionStruct& operator = (const ResultValueDefinitionStruct& rhs);
private:
	void Clear();
};
#pragma pack (pop)

} //namespace SvOp
