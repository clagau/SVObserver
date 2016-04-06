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

#pragma pack (push, 1)
struct InputValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;
	BSTR bstrHelpText;
	BSTR bstrGroup;
	_variant_t vDefaultValue;
public:
	InputValueDefinitionStruct( );
	~InputValueDefinitionStruct( );
	InputValueDefinitionStruct(const InputValueDefinitionStruct& rhs );
	const InputValueDefinitionStruct& operator = (const InputValueDefinitionStruct& rhs );
	bool operator == (const InputValueDefinitionStruct& rhs);
private:
	void Clear();
	void Init();
};
#pragma pack (pop)


typedef std::vector<InputValueDefinitionStruct> InputValueDefinitionStructArray;

#pragma pack (push, 1)
struct ResultValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;	// not used at this time
public:
	ResultValueDefinitionStruct( );
	~ResultValueDefinitionStruct( );
	ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rhs );
	const ResultValueDefinitionStruct& operator = (const ResultValueDefinitionStruct& rhs );
private:
	void Clear();
	void Init();
};
#pragma pack (pop)

typedef std::vector<ResultValueDefinitionStruct> ResultValueDefinitionStructArray;

