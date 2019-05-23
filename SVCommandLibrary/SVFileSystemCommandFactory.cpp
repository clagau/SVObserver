//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemCommandFactory
//* .File Name       : $Workfile:   SVFileSystemCommandFactory.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVFileSystemCommandFactory.h"

#include "SVCommandConcreteTemplate.h"
#include "SVCommandConcreteFactoryTemplate.h"
#include "SVCommandDataValue.h"
#include "SVCommandDataHolder.h"
#pragma endregion Includes

typedef SVCommandConcreteTemplate< 0x00020001 > SVDoesFileExistCommand;
const SVDoesFileExistCommand::SVCommandAttributes::SVDefaultAttributes SVDoesFileExistCommand::SVCommandAttributes::m_DefaultAttributes
{
	{_T( "File Name" ),  SVCommandDataFacadePtr( new SVCommandDataValue )}
};

typedef SVCommandConcreteFactoryTemplate< SVDoesFileExistCommand > SVDoesFileExistFactory;

SVFileSystemCommandFactory::SVCommandFactoryMap SVFileSystemCommandFactory::m_DefaultFactories
{
	{SVDoesFileExistFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVDoesFileExistFactory( boost::bind( &SVFileSystemCommandFactory::DoesFileExist, _1, _2 ) ) )}
};

HRESULT SVFileSystemCommandFactory::DoesFileExist( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Temp;

	l_Status = p_rAttributes.GetValue( _T( "File Name" ), l_Temp );

	if( S_OK == l_Status )
	{
		FILE *fFile;

		//use fopen to see if the file can be open
		//if so return S_OK,else check
		fFile = fopen( static_cast< char* >( _bstr_t( l_Temp ) ), "r" );

		if( fFile )
		{
			fclose( fFile );
		}
		else
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}

SVFileSystemCommandFactory::SVFileSystemCommandFactory()
: SVCommandAbstractFactoryTemplate( m_DefaultFactories )
{
}

SVFileSystemCommandFactory::~SVFileSystemCommandFactory()
{
}

