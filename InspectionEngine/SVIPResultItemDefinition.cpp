//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemDefinition
//* .File Name       : $Workfile:   SVIPResultItemDefinition.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:45:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPResultItemDefinition.h"

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVIPResultItemDefinition::SVIPResultItemDefinition()
: m_ObjectID(), m_IndexPresent( false ), m_Index(0)
{
}

SVIPResultItemDefinition::SVIPResultItemDefinition( const SVGUID& p_rObjectID )
: m_ObjectID( p_rObjectID ), m_IndexPresent( false ), m_Index(0)
{
}

SVIPResultItemDefinition::SVIPResultItemDefinition( const SVGUID& p_rObjectID, int index )
: m_ObjectID( p_rObjectID ), m_IndexPresent( true ), m_Index( index )
{
}

SVIPResultItemDefinition::SVIPResultItemDefinition( const SVIPResultItemDefinition& p_rObject )
: m_ObjectID( p_rObject.m_ObjectID ), m_IndexPresent( p_rObject.m_IndexPresent ), m_Index( p_rObject.m_Index )
{
}

bool SVIPResultItemDefinition::operator<( const SVIPResultItemDefinition& p_rObject ) const
{
	bool l_Status = ( m_ObjectID < p_rObject.m_ObjectID );

	if( !l_Status )
	{
		if( m_ObjectID == p_rObject.m_ObjectID )
		{
			if( m_IndexPresent == p_rObject.m_IndexPresent )
			{
				if( m_IndexPresent )
				{

					l_Status = ( m_Index < p_rObject.m_Index );
				}
			}
			else
			{
				l_Status = ( m_IndexPresent < p_rObject.m_IndexPresent );
			}
		}
	}

	return l_Status;
}

bool SVIPResultItemDefinition::operator==( const SVIPResultItemDefinition& p_rObject ) const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_ObjectID == p_rObject.m_ObjectID );
	l_Status = l_Status && ( m_IndexPresent == p_rObject.m_IndexPresent );
	l_Status = l_Status && ( m_Index == p_rObject.m_Index );

	return l_Status;
}

const SVGUID& SVIPResultItemDefinition::GetObjectID() const
{
	return m_ObjectID;
}

bool SVIPResultItemDefinition::GetIndexPresent() const
{
	return m_IndexPresent;
}

 int SVIPResultItemDefinition::GetIndex() const
{
	return m_Index;
}

} //namespace SvIe
