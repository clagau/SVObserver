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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultItemDefinition.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:45:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Jul 2012 14:37:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed operator <(const SVIPResultItemDefinition&) to only look at m_IndexPresent for items with the same object ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/