// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader
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
		m_Published = rhs.m_Published;
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
	m_Name = SVString::EmptyString();
	m_Writable = false;
	m_Published = false;
	m_Type = SVString::EmptyString();
	m_AdditionalInfo.clear();
}

bool SVDataDefinitionStruct::operator == (const SVDataDefinitionStruct& rhs)
{
	bool bEqual = true;
	
	bEqual = bEqual && (m_Name == rhs.m_Name);
	bEqual = bEqual && (m_Writable == rhs.m_Writable);
	bEqual = bEqual && (m_Published == rhs.m_Published);
	bEqual = bEqual && (m_Type == rhs.m_Type);

	return bEqual;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDataDefinitionStruct.cpp_v  $
 * 
 *    Rev 1.0   04 Nov 2013 14:35:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/