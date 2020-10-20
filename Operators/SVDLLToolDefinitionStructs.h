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
#include "ObjectInterfaces/IInputValueDefinition.h"
#include "ObjectInterfaces/IResultValueDefinition.h"
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes


///Operators
namespace SvOp
{
	const static DWORD ERROR_USER_EXCEPTION{ 0xFF00000F };

/// The structure definition must be the same as the one in the External dll
/// Use shared files?
#pragma pack (push, 1)

struct DefinitionStructEx
{
	long vt {VT_EMPTY};
	_bstr_t Name;
	_bstr_t HelpText;
	_bstr_t Group;
	_variant_t Reseved1; // not used at this time
	_variant_t Reserved2; // not used at this time
};

struct InputValueDefinitionStructEx : public DefinitionStructEx
{
	_variant_t vDefaultValue;
};

struct ResultValueDefinitionStructEx : public DefinitionStructEx
{
	long ArraySize {20};
};

struct ResultTableDefinitionStructEx : public DefinitionStructEx
{
	long ColoumnCount {0};
	long RowCount {0};
	_variant_t ColumnNames; //smart array of bstr with names
	
};

#pragma pack (pop)


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
/// contents the structure from the dll with some additional calculated information 
class  InputValueDefinition : public SvOi::IInputValueDefinition
{
public:
	InputValueDefinition() = default;
	virtual ~InputValueDefinition() = default;

	virtual long getVt() const override;
	virtual std::string getDisplayName() const override;
	virtual std::string getHelpText() const override;
	virtual std::string getGroup() const override;
	virtual const _variant_t& getDefaultValue() const override;
	//Throw exception if InputValueDefinitionStruct is not allowed
	void setDefinition(const InputValueDefinitionStruct&  InputValueDefStruct, long* NofLinkedValue);
	void setDefinition(const InputValueDefinitionStructEx&  InputValueDefStruct, long* pNLValue);
	virtual int getDim() const override { return m_Dim; };
	virtual SvPb::ExDllInterfaceType getType() const override { return m_Type; };
	int  getLinkedValueIndex() const { return m_LinkedValueIndex; };
	bool UseDisplayNames() const { return m_UseDisplaynames; };
private:
	int m_LinkedValueIndex {-1};
	
	long m_VT;
	std::string  m_DisplayName;
	std::string  m_HelpText;
	std::string  m_Group;
	_variant_t m_DefaultValue;
	bool m_UseDisplaynames;
	int							m_Dim {0}; //dimension of Arrays
	SvPb::ExDllInterfaceType             m_Type{ SvPb::ExDllInterfaceType::Scalar };
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




//!class describing the resultvalue definitions for External dll
/// contents the structure from the dll with some additional calculated information 
class ResultValueDefinition : public SvOi::IResultValueDefinition
{
public:
	ResultValueDefinition() = default;
	~ResultValueDefinition() = default;

	void setDefinition(const ResultValueDefinitionStruct&  resultValueDefinitionStruct, long ValueIndex);
	void setDefinition(const ResultValueDefinitionStructEx&  resultValueDefinitionStruct, long ValueIndex);
	int getIndex() const;
	virtual std::string getDisplayName() const override;
	virtual long getVT() const override;
	long getMaxArraysize() const;
	void setMaxArraysize(long);
	virtual bool UseDisplayNames() const override { return m_UseDisplayNames; };
	virtual std::string getGroup() const override { return m_Group; }
	virtual std::string  getHelpText() const override { return m_HelpText; }
	
private:
	int m_ValueIndex {-1};
	int m_MaxArraysize {20};
	long m_VT {VT_EMPTY};
	std::string  m_DisplayName;	
	std::string  m_HelpText;
	std::string  m_Group;
	bool m_UseDisplayNames {false};
};

//!class describing the resulttable definitions for External dll
/// contents the structure from the dll with some additional calculated information 
class ResultTableDefinition 
{
public:
	ResultTableDefinition() = default;
	~ResultTableDefinition() = default;

	long getTableColoumnCount() const { return m_ColoumnCount; };
	long getTableRowCount() const { return m_RowCount; };
	//Throw exception if ResultValueDefinitionStruct is not allowed
	void setDefinition(const ResultTableDefinitionStruct&  DefinitionStruct, long ValueIndex);
	void setDefinition(const ResultTableDefinitionStructEx&  DefinitionStruct, long ValueIndex);
	std::vector<std::string> getColoumnNames() const;

	int getIndex() const { return m_ValueIndex; };
	std::string getDisplayName() const;
	long getVT() const { return m_vt; };
	void  setTableRowCount(long count ) {  m_RowCount = count; };
	bool UseDisplayNames() const { return m_UseDisplayNames; };
	std::string  getGroup() const { return m_Group; }
	std::string getHelpText() const { return m_HelpText; }
private:
	
	long m_vt {VT_EMPTY};
	long m_ColoumnCount {0};
	long m_RowCount {0};
	_variant_t m_ColumnNames; ///<smart array of bstr with rownames
	bool  m_UseDisplayNames {false};
	int m_ValueIndex {-1};
	std::string  m_HelpText;
	std::string  m_Group;
	std::string m_DisplayName;
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
