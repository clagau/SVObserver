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

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include "SVFileSystemCommandFactory.h"

#include "SVCommandLibrary/SVCommandConcreteTemplate.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataValue.h"
#include "SVCommandLibrary/SVCommandConcreteFactoryTemplate.h"

typedef SVCommandConcreteTemplate< 0x00020001 > SVDoesFileExistCommand;
const SVDoesFileExistCommand::SVCommandAttributes::SVDefaultAttributes SVDoesFileExistCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "File Name" ),  SVCommandDataFacadePtr( new SVCommandDataValue ) );
typedef SVCommandConcreteFactoryTemplate< SVDoesFileExistCommand > SVDoesFileExistFactory;

SVFileSystemCommandFactory::SVCommandFactoryMap SVFileSystemCommandFactory::m_DefaultFactories = boost::assign::map_list_of<>
	( SVDoesFileExistFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVDoesFileExistFactory( boost::bind( &SVFileSystemCommandFactory::DoesFileExist, _1, _2 ) ) ) );

HRESULT SVFileSystemCommandFactory::DoesFileExist( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Temp;

	l_Status = p_rAttributes.GetValue( _T( "File Name" ), l_Temp );

	if( l_Status == S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileSystemLibrary\SVFileSystemCommandFactory.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:37:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Mar 2011 13:32:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to handle file system commands from remote systems via SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:00:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
