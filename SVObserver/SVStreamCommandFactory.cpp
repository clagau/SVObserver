//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStreamCommandFactory
//* .File Name       : $Workfile:   SVStreamCommandFactory.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:48  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include "SVStreamCommandFactory.h"

#include "SVCommandLibrary/SVCommandConcreteTemplate.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataValue.h"
#include "SVCommandLibrary/SVCommandConcreteFactoryTemplate.h"

#include "SVInputStreamManager.h"

typedef SVCommandConcreteTemplate< 0x00030001 > SVQueryInputStreamNamesCommand;
const SVQueryInputStreamNamesCommand::SVCommandAttributes::SVDefaultAttributes SVQueryInputStreamNamesCommand::SVCommandAttributes::m_DefaultAttributes = SVQueryInputStreamNamesCommand::SVCommandAttributes::SVDefaultAttributes();
typedef SVCommandConcreteFactoryTemplate< SVQueryInputStreamNamesCommand > SVQueryInputStreamNamesFactory;

typedef SVCommandConcreteTemplate< 0x00030002 > SVQueryInputStreamDataItemsCommand;
const SVQueryInputStreamDataItemsCommand::SVCommandAttributes::SVDefaultAttributes SVQueryInputStreamDataItemsCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Stream Name" ),  SVCommandDataFacadePtr( new SVCommandDataValue ) );
typedef SVCommandConcreteFactoryTemplate< SVQueryInputStreamDataItemsCommand > SVQueryInputStreamDataItemsFactory;

typedef SVCommandConcreteTemplate< 0x00030003 > SVClearInputStreamDataItemsCommand;
const SVClearInputStreamDataItemsCommand::SVCommandAttributes::SVDefaultAttributes SVClearInputStreamDataItemsCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Stream Name" ),  SVCommandDataFacadePtr( new SVCommandDataValue ) );
typedef SVCommandConcreteFactoryTemplate< SVClearInputStreamDataItemsCommand > SVClearInputStreamDataItemsFactory;

SVStreamCommandFactory::SVCommandFactoryMap SVStreamCommandFactory::m_DefaultFactories = boost::assign::map_list_of<>
	( SVQueryInputStreamNamesFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVQueryInputStreamNamesFactory( boost::bind( &SVStreamCommandFactory::QueryInputStreamNames, _1, _2 ) ) ) )
	( SVQueryInputStreamDataItemsFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVQueryInputStreamDataItemsFactory( boost::bind( &SVStreamCommandFactory::QueryInputStreamDataItems, _1, _2 ) ) ) )
	( SVClearInputStreamDataItemsFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVClearInputStreamDataItemsFactory( boost::bind( &SVStreamCommandFactory::ClearInputStreamDataItems, _1, _2 ) ) ) );

HRESULT SVStreamCommandFactory::QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	return SVInputStreamManager::Instance().QueryInputStreamNames( p_rAttributes, p_rResults );
}

HRESULT SVStreamCommandFactory::QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	return SVInputStreamManager::Instance().QueryInputStreamDataItems( p_rAttributes, p_rResults );
}

HRESULT SVStreamCommandFactory::ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	return SVInputStreamManager::Instance().ClearInputStreamDataItems( p_rAttributes, p_rResults );
}

SVStreamCommandFactory::SVStreamCommandFactory()
: SVCommandAbstractFactoryTemplate( m_DefaultFactories )
{
}

SVStreamCommandFactory::~SVStreamCommandFactory()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStreamCommandFactory.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:14:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Feb 2013 14:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:39:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new stream manager to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
