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


InputValueDefinitionStruct::InputValueDefinitionStruct( )
{
	Init();
}

InputValueDefinitionStruct::InputValueDefinitionStruct(const InputValueDefinitionStruct& rhs )
{
	Init();
	*this = rhs;
}

InputValueDefinitionStruct::~InputValueDefinitionStruct( )
{
	Clear();
}

const InputValueDefinitionStruct& InputValueDefinitionStruct::operator = (const InputValueDefinitionStruct& rhs )
{

	if( this != &rhs )
	{
		Clear();
		lVT  = rhs.lVT;
		vDefaultValue = rhs.vDefaultValue;
		bstrDisplayName = CString(rhs.bstrDisplayName).AllocSysString();
		bstrHelpText = CString (rhs.bstrHelpText).AllocSysString();
		bstrGroup = CString (rhs.bstrGroup).AllocSysString();
	}
	return *this;
}

void InputValueDefinitionStruct::Clear( )
{
	lVT  = VT_EMPTY;
	vDefaultValue.Clear();
	if(bstrDisplayName)
		::SysFreeString(bstrDisplayName);
	if(bstrHelpText )
		::SysFreeString(bstrHelpText);
	if(bstrGroup )
		::SysFreeString(bstrGroup);
	Init();
}

void InputValueDefinitionStruct::Init( )
{
	lVT = VT_EMPTY;
	bstrDisplayName = nullptr;
	bstrHelpText = nullptr;
	bstrGroup = nullptr;
}

bool InputValueDefinitionStruct::operator == (const InputValueDefinitionStruct& rhs)
{
	bool bEqual = true;
	
	bEqual = bEqual && (lVT == rhs.lVT);
	bEqual = bEqual && (0 == wcscmp( bstrDisplayName, rhs.bstrDisplayName ));
	bEqual = bEqual && (0 == wcscmp( bstrHelpText, rhs.bstrHelpText ));
	bEqual = bEqual && (0 == wcscmp( bstrGroup, rhs.bstrGroup ));
	bEqual = bEqual && (vDefaultValue == rhs.vDefaultValue);

	return bEqual;
}



// ResultValueDefinitionStruct
ResultValueDefinitionStruct::ResultValueDefinitionStruct( )
{
	Init();
}


ResultValueDefinitionStruct::ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rhs )
{
	Init();
	*this = rhs;
}

ResultValueDefinitionStruct::~ResultValueDefinitionStruct( )
{
	Clear();
}

const ResultValueDefinitionStruct& ResultValueDefinitionStruct::operator = (const ResultValueDefinitionStruct& rhs )
{

	if( this != &rhs )
	{
		Clear();
		lVT  = rhs.lVT;
		bstrDisplayName = CString(rhs.bstrDisplayName).AllocSysString();
	}
	return *this;
}

void ResultValueDefinitionStruct::Clear( )
{
	lVT  = VT_EMPTY;
	if(bstrDisplayName)
		::SysFreeString(bstrDisplayName);

}

void ResultValueDefinitionStruct::Init( )
{
	lVT = VT_EMPTY;
	bstrDisplayName = nullptr;
}

