//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactoryTemplate
//* .File Name       : $Workfile:   SVCommandAbstractFactoryTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandAbstractFactoryTemplate.h"

SVCommandAbstractFactoryTemplate::SVCommandAbstractFactoryTemplate( const SVCommandFactoryMap& p_rObject )
: m_Factories( p_rObject )
{
}

SVCommandAbstractFactoryTemplate::~SVCommandAbstractFactoryTemplate()
{
}

bool SVCommandAbstractFactoryTemplate::IsValidCommandType( unsigned long p_CommandType ) const
{
	SVCommandFactoryMap::const_iterator l_Iter = m_Factories.find( p_CommandType );

	return ( l_Iter != m_Factories.end() );
}

SVCommandTemplatePtr SVCommandAbstractFactoryTemplate::CreateCommand( unsigned long p_CommandType ) const
{
	SVCommandTemplatePtr l_pCommand;

	SVCommandFactoryMap::const_iterator l_Iter = m_Factories.find( p_CommandType );

	if( l_Iter != m_Factories.end() )
	{
		if( ! l_Iter->second.empty() )
		{
			l_pCommand = l_Iter->second->CreateCommand();
		}
	}

	return l_pCommand;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandAbstractFactoryTemplate.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
