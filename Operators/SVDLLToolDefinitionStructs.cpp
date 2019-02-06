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

InputValueDefinitionStruct::InputValueDefinitionStruct( ) :
  m_VT( VT_EMPTY)
, m_bDisplayName( nullptr )
, m_bGroup( nullptr )
, m_bHelpText( nullptr )
{
}

InputValueDefinitionStruct::InputValueDefinitionStruct(const InputValueDefinitionStruct& rRhs ) :
  m_VT( VT_EMPTY)
, m_bDisplayName( nullptr )
, m_bGroup( nullptr )
, m_bHelpText( nullptr )
{
	*this = rRhs;
}

InputValueDefinitionStruct::~InputValueDefinitionStruct( )
{
	Clear();
}

const InputValueDefinitionStruct& InputValueDefinitionStruct::operator = (const InputValueDefinitionStruct& rRhs )
{

	if( this != &rRhs )
	{
		Clear();
		m_VT  = rRhs.m_VT;
		m_DefaultValue = rRhs.m_DefaultValue;
		m_bDisplayName = _bstr_t(rRhs.m_bDisplayName).copy();
		m_bGroup = _bstr_t(rRhs.m_bGroup).copy();
		m_bHelpText = _bstr_t(rRhs.m_bHelpText).copy();
	}
	return *this;
}

void InputValueDefinitionStruct::Clear( )
{
	m_VT  = VT_EMPTY;
	m_DefaultValue.Clear();
	if( nullptr != m_bDisplayName )
	{
		::SysFreeString( m_bDisplayName );
		m_bDisplayName = nullptr;
	}
	if( nullptr != m_bGroup )
	{
		::SysFreeString( m_bGroup );
		m_bGroup = nullptr;
	}
	if( nullptr != m_bHelpText )
	{
		::SysFreeString( m_bHelpText );
		m_bHelpText = nullptr;
	}
}

bool InputValueDefinitionStruct::operator == (const InputValueDefinitionStruct& rRhs)
{
	bool bEqual = true;
	
	bEqual = bEqual && ( m_VT == rRhs.m_VT );
	bEqual = bEqual && ( _bstr_t(m_bDisplayName) == _bstr_t(rRhs.m_bDisplayName) );
	bEqual = bEqual && ( _bstr_t(m_bGroup) == _bstr_t(rRhs.m_bGroup) );
	bEqual = bEqual && ( _bstr_t(m_bHelpText) == _bstr_t(rRhs.m_bHelpText) );
	bEqual = bEqual && ( _bstr_t(m_DefaultValue) == _bstr_t(rRhs.m_DefaultValue) );

	return bEqual;
}



// ResultValueDefinitionStruct
ResultValueDefinitionStruct::ResultValueDefinitionStruct( ) :
  m_VT( VT_EMPTY)
, m_bDisplayName( nullptr )
{
}

ResultValueDefinitionStruct::ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rRhs ) :
  m_VT( VT_EMPTY)
, m_bDisplayName( nullptr )
{
	*this = rRhs;
}

ResultValueDefinitionStruct::~ResultValueDefinitionStruct( )
{
	Clear();
}

const ResultValueDefinitionStruct& ResultValueDefinitionStruct::operator = (const ResultValueDefinitionStruct& rRhs )
{

	if( this != &rRhs )
	{
		Clear();
		m_VT  = rRhs.m_VT;
		m_bDisplayName = _bstr_t(rRhs.m_bDisplayName).copy();
	}
	return *this;
}

void ResultValueDefinitionStruct::Clear( )
{
	m_VT  = VT_EMPTY;
	if( nullptr != m_bDisplayName )
	{
		::SysFreeString( m_bDisplayName );
		m_bDisplayName = nullptr;
	}
}

} //namespace SvOp
