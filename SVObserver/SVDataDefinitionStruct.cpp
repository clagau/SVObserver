// ******************************************************************************
// * COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataDefinitionStruct
// * .File Name       : $Workfile:   SVDataDefinitionStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   04 Nov 2013 14:35:26  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDataDefinitionStruct.h"

SVDataDefinitionStruct::SVDataDefinitionStruct( )
{
	Init();
}

SVDataDefinitionStruct::SVDataDefinitionStruct(const SVDataDefinitionStruct& rhs )
{
	Init();
	*this = rhs;
}

SVDataDefinitionStruct::~SVDataDefinitionStruct( )
{
	Clear();
}

const SVDataDefinitionStruct& SVDataDefinitionStruct::operator = (const SVDataDefinitionStruct& rhs )
{

	if( this != &rhs )
	{
		Clear();
		m_Name  = rhs.m_Name;
		m_Writable = rhs.m_Writable;
		m_Type = rhs.m_Type;
		if ( m_AdditionalInfo.size() != rhs.m_AdditionalInfo.size() )
		{
			m_AdditionalInfo.resize( rhs.m_AdditionalInfo.size() );
		}
		std::copy( rhs.m_AdditionalInfo.begin(), rhs.m_AdditionalInfo.end(), m_AdditionalInfo.begin() );
	}
	return *this;
}

void SVDataDefinitionStruct::Clear( )
{
	Init();
}

void SVDataDefinitionStruct::Init( )
{
	m_Name = _T("");
	m_Writable = false;
	m_Type = _T("");
	m_AdditionalInfo.clear();
}

bool SVDataDefinitionStruct::operator == (const SVDataDefinitionStruct& rhs)
{
	bool bEqual = true;
	
	bEqual = bEqual && (m_Name == rhs.m_Name);
	bEqual = bEqual && (m_Writable == rhs.m_Writable);
	bEqual = bEqual && (m_Type == rhs.m_Type);

	return bEqual;
}

