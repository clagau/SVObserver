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

