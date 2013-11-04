//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplateFacade
//* .File Name       : $Workfile:   SVCommandDataTemplateFacade.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:26  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATEFACADE_INL
#define SVCOMMANDDATATEMPLATEFACADE_INL

#include "SVCommandDataTemplateFacade.h"

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplateFacade()
: m_DataType( p_BaseDataType )
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::SVCommandDataTemplateFacade( bool p_ReadOnly )
: m_DataType( p_BaseDataType | ( p_ReadOnly ? SV_COMMAND_DATA_TYPE_READ_ONLY : 0 ) )
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::~SVCommandDataTemplateFacade()
{
}

template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType >
unsigned long SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >::GetDataType() const
{
	return m_DataType;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataTemplateFacade.inl_v  $
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
 *    Rev 1.0   15 Oct 2010 14:32:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
