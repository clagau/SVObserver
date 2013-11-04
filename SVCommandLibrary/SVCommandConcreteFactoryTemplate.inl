//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandFactoryTemplate
//* .File Name       : $Workfile:   SVCommandConcreteFactoryTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:16  $
//******************************************************************************

#ifndef SVCOMMANDFACTORYTEMPLATE_INL
#define SVCOMMANDFACTORYTEMPLATE_INL

#include "SVCommandConcreteFactoryTemplate.h"

template< typename SVCommandObject >
unsigned long SVCommandConcreteFactoryTemplate< SVCommandObject >::GetStaticCommandType()
{
	return SVCommandObject::GetStaticCommandType();
}

template< typename SVCommandObject >
SVCommandConcreteFactoryTemplate< SVCommandObject >::SVCommandConcreteFactoryTemplate( SVCommandExecuteFunctor p_ExecuteFunctor )
: SVCommandFactoryTemplate( SVCommandObject::GetStaticCommandType() ), m_ExecuteFunctor( p_ExecuteFunctor )
{
}

template< typename SVCommandObject >
SVCommandConcreteFactoryTemplate< SVCommandObject >::~SVCommandConcreteFactoryTemplate()
{
}

template< typename SVCommandObject >
SVCommandTemplatePtr SVCommandConcreteFactoryTemplate< SVCommandObject >::CreateCommand() const
{
	return new SVCommandObject( m_ExecuteFunctor );
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandConcreteFactoryTemplate.inl_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
