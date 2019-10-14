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
	InputValueDefinitionStruct& operator = (const InputValueDefinitionStruct& rhs);
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

	long getVt() const;
	std::string getDisplayName() const;
	std::string getHelpText() const;
	std::string getGroup() const;
	const _variant_t& getDefaultValue() const;
	//Throw exception if InputValueDefinitionStruct is not allowed
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
	ResultValueDefinitionStruct& operator = (const ResultValueDefinitionStruct& rhs);
private:
	void Clear();
};

#pragma pack (pop)




#pragma pack (push, 1)
struct ResultTableDefinitionStruct
{
	ResultTableDefinitionStruct& operator = (const ResultTableDefinitionStruct& rhs);
	ResultTableDefinitionStruct(const ResultTableDefinitionStruct& rhs);
	ResultTableDefinitionStruct() = default;
	~ResultTableDefinitionStruct() = default;
	long lVT {VT_EMPTY};
	_bstr_t bstrDisplayName;	// not used at this time
	DWORD type {0}; 	// not used at this time
	long ColoumnCount {0};
	long RowCount {0};
	_variant_t ColumnNames; //smart array of bstr with rownames
};

#pragma pack (pop)

class ResultValueDefinition
{
public:
	ResultValueDefinition() = default;
	~ResultValueDefinition() = default;

	//Throw exception if ResultValueDefinitionStruct is not allowed
	void setDefinition(const ResultValueDefinitionStruct&  resultValueDefinitionStruct, long ValueIndex);
	int getIndex() const;
	std::string getDisplayName() const;
	SvOp::ExDllInterfaceType getType() const;
	long getVT() const;
	long getMaxArraysize() const;
	void setMaxArraysize(long);
	
private:
	int m_ValueIndex {-1};
	int m_MaxArraysize {20};

	ResultValueDefinitionStruct m_ValueDefinition;
};

class ResultTableDefinition
{
public:
	ResultTableDefinition() = default;
	~ResultTableDefinition() = default;

	long getTableColoumnCount() const { return m_TableDefinition.ColoumnCount; };
	long getTableRowCount() const { return m_TableDefinition.RowCount; };
	//Throw exception if ResultValueDefinitionStruct is not allowed
	void setDefinition(const ResultTableDefinitionStruct&  DefinitionStruct, long ValueIndex);
	std::vector<std::string> getColoumnNames() const;

	int getIndex() const { return m_ValueIndex; };
	std::string getDisplayName() const;
	SvOp::ExDllInterfaceType getType() const;
	long getVT() const { return m_TableDefinition.lVT; };
	void  setTableRowCount(long count ) {  m_TableDefinition.RowCount = count; };
	
private:

	ResultTableDefinitionStruct m_TableDefinition;
	int m_ValueIndex {-1};
};


struct InputImageInformationStruct
{
	_bstr_t DisplayName="";
	_bstr_t HelpText = "";
	///AllowedImageTypes is meant to contain flags that specify which image types are acceptable.
	/// currently, only two bits are used: 
	///		bit 0 allows black and white images
	///		bit 32 allows color images
	/// later, bits 1 to 31 could specify specific black and white image types and
	/// later, bits 33 to 63 could specify specific color image types
	uint64_t AllowedImageTypes = 0;
	
	void allowBlackAndWhite();
	void allowColor();
	bool mayBeBlackAndWhite() const;
	bool mayBeColor() const;
};


} //namespace SvOp
