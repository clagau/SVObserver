//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandConcreteTemplate
//* .File Name       : $Workfile:   SVCommandConcreteTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:26  $
//******************************************************************************

#ifndef SVCOMMANDCONCRETETEMPLATE_INL
#define SVCOMMANDCONCRETETEMPLATE_INL

#include "SVCommandConcreteTemplate.h"

template< unsigned long p_CommandType >
unsigned long SVCommandConcreteTemplate< p_CommandType >::GetStaticCommandType()
{
	return p_CommandType;
}

template< unsigned long p_CommandType >
SVCommandConcreteTemplate< p_CommandType >::SVCommandConcreteTemplate( SVCommandExecuteFunctor p_ExecuteFunctor )
: SVCommandTemplate( p_CommandType )
, SVCommandAttributes()
, SVCommandResults()
, m_ExecuteFunctor( p_ExecuteFunctor )
{
}

template< unsigned long p_CommandType >
SVCommandConcreteTemplate< p_CommandType >::~SVCommandConcreteTemplate()
{
}

template< unsigned long p_CommandType >
HRESULT SVCommandConcreteTemplate< p_CommandType >::Execute()
{
	return m_ExecuteFunctor( m_Attributes, m_Results );
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandConcreteTemplate.inl_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:30:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
