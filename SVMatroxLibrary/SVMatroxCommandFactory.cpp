//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandFactory
//* .File Name       : $Workfile:   SVMatroxCommandFactory.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:12:14  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include "SVMatroxCommandFactory.h"

#include "SVCommandLibrary/SVCommandConcreteTemplate.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataValue.h"
#include "SVCommandLibrary/SVCommandConcreteFactoryTemplate.h"

#include "SVMatroxCommandDataImage.h"
#include "SVMatroxImageInterface.h"
#include "SVMatroxOcrInterface.h"

typedef SVCommandConcreteTemplate< 0x00010001 > SVAdaptiveThresholdCommand;
const SVAdaptiveThresholdCommand::SVCommandAttributes::SVDefaultAttributes SVAdaptiveThresholdCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Source Image" ),         SVCommandDataFacadePtr( new SVMatroxCommandDataImage           ) )
	( _T( "Is Black Background" ),  SVCommandDataFacadePtr( new SVCommandDataValue( bool( true ) ) ) );
typedef SVCommandConcreteFactoryTemplate< SVAdaptiveThresholdCommand > SVAdaptiveThresholdFactory;

typedef SVCommandConcreteTemplate< 0x00010002 > SVAutoThresholdCommand;
const SVAutoThresholdCommand::SVCommandAttributes::SVDefaultAttributes SVAutoThresholdCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Source Image" ),         SVCommandDataFacadePtr( new SVMatroxCommandDataImage            ) )
	( _T( "Threshold Multiplier" ), SVCommandDataFacadePtr( new SVCommandDataValue( double( 1.0 ) ) ) )
	( _T( "Is Black Background" ),  SVCommandDataFacadePtr( new SVCommandDataValue( bool( true ) )  ) );
typedef SVCommandConcreteFactoryTemplate< SVAutoThresholdCommand > SVAutoThresholdFactory;

typedef SVCommandConcreteTemplate< 0x00010003 > SVFixedThresholdCommand;
const SVFixedThresholdCommand::SVCommandAttributes::SVDefaultAttributes SVFixedThresholdCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Source Image" ),          SVCommandDataFacadePtr( new SVMatroxCommandDataImage                   ) )
	( _T( "Threshold Lower Value" ), SVCommandDataFacadePtr( new SVCommandDataValue( unsigned char( 128 ) ) ) )
	( _T( "Threshold Upper Value" ), SVCommandDataFacadePtr( new SVCommandDataValue( unsigned char( 0 ) )   ) );
typedef SVCommandConcreteFactoryTemplate< SVFixedThresholdCommand > SVFixedThresholdFactory;

typedef SVCommandConcreteTemplate< 0x00010004 > SVFindFontCharactersCommand;
const SVFindFontCharactersCommand::SVCommandAttributes::SVDefaultAttributes SVFindFontCharactersCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Source Image" ),        SVCommandDataFacadePtr( new SVMatroxCommandDataImage           ) )
	( _T( "Width" ),               SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) )    ) )
	( _T( "Height" ),              SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) )    ) )
	( _T( "Is Black Background" ), SVCommandDataFacadePtr( new SVCommandDataValue( bool( false ) ) ) );
typedef SVCommandConcreteFactoryTemplate< SVFindFontCharactersCommand > SVFindFontCharactersFactory;

typedef SVCommandConcreteTemplate< 0x00010005 > SVLoadFontCommand;
const SVLoadFontCommand::SVCommandAttributes::SVDefaultAttributes SVLoadFontCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Character File Contents" ),  SVCommandDataFacadePtr( new SVCommandDataValue ) );
typedef SVCommandConcreteFactoryTemplate< SVLoadFontCommand > SVLoadFontFactory;

typedef SVCommandConcreteTemplate< 0x00010006 > SVSaveFontCommand;
const SVSaveFontCommand::SVCommandAttributes::SVDefaultAttributes SVSaveFontCommand::SVCommandAttributes::m_DefaultAttributes = SVSaveFontCommand::SVCommandAttributes::SVDefaultAttributes();
typedef SVCommandConcreteFactoryTemplate< SVSaveFontCommand > SVSaveFontFactory;

typedef SVCommandConcreteTemplate< 0x00010007 > SVCalibrateFontCommand;
const SVCalibrateFontCommand::SVCommandAttributes::SVDefaultAttributes SVCalibrateFontCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Calibrate Image" ),               SVCommandDataFacadePtr( new SVMatroxCommandDataImage        ) )
	( _T( "Calibrate String" ),              SVCommandDataFacadePtr( new SVCommandDataValue              ) )
	( _T( "Maximum Character Size Width" ),  SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Minimum Character Size Width" ),  SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Maximum Character Size Height" ), SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Minimum Character Size Height" ), SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Character Step Size Width" ),     SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Character Step Size Height" ),    SVCommandDataFacadePtr( new SVCommandDataValue( long( 0 ) ) ) )
	( _T( "Font" ),                          SVCommandDataFacadePtr( new SVCommandDataContainer          ) );
typedef SVCommandConcreteFactoryTemplate< SVCalibrateFontCommand > SVCalibrateFontFactory;

typedef SVCommandConcreteTemplate< 0x00010008 > SVReadFontCommand;
const SVReadFontCommand::SVCommandAttributes::SVDefaultAttributes SVReadFontCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Read Image" ), SVCommandDataFacadePtr( new SVMatroxCommandDataImage ) )
	( _T( "Font" ),       SVCommandDataFacadePtr( new SVCommandDataContainer   ) );
typedef SVCommandConcreteFactoryTemplate< SVReadFontCommand > SVReadFontFactory;

typedef SVCommandConcreteTemplate< 0x00010009 > SVVerifyFontCommand;
const SVVerifyFontCommand::SVCommandAttributes::SVDefaultAttributes SVVerifyFontCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Verify Image" ),  SVCommandDataFacadePtr( new SVMatroxCommandDataImage     ) )
	( _T( "Verify String" ), SVCommandDataFacadePtr( new SVCommandDataValue           ) )
	( _T( "Font" ),          SVCommandDataFacadePtr( new SVCommandDataContainer       ) );
typedef SVCommandConcreteFactoryTemplate< SVVerifyFontCommand > SVVerifyFontFactory;

typedef SVCommandConcreteTemplate< 0x0001000A > SVCharacterThicknessCommand;
const SVCharacterThicknessCommand::SVCommandAttributes::SVDefaultAttributes SVCharacterThicknessCommand::SVCommandAttributes::m_DefaultAttributes = boost::assign::map_list_of<>
	( _T( "Threshold Image" ), SVCommandDataFacadePtr( new SVMatroxCommandDataImage ) )
	( _T( "Background Type" ), SVCommandDataFacadePtr( new SVCommandDataValue       ) );
typedef SVCommandConcreteFactoryTemplate< SVCharacterThicknessCommand > SVCharacterThicknessFactory;

SVMatroxCommandFactory::SVCommandFactoryMap SVMatroxCommandFactory::m_DefaultFactories = boost::assign::map_list_of<>
	( SVAdaptiveThresholdFactory::GetStaticCommandType(),  SVCommandFactoryTemplatePtr( new SVAdaptiveThresholdFactory( boost::bind( &SVMatroxImageInterface::AdaptiveThreshold, _1, _2 ) ) ) )
	( SVAutoThresholdFactory::GetStaticCommandType(),      SVCommandFactoryTemplatePtr( new SVAutoThresholdFactory( boost::bind( &SVMatroxImageInterface::AutoThreshold, _1, _2 ) )         ) )
	( SVFixedThresholdFactory::GetStaticCommandType(),     SVCommandFactoryTemplatePtr( new SVFixedThresholdFactory( boost::bind( &SVMatroxImageInterface::FixedThreshold, _1, _2 ) )       ) )
	( SVFindFontCharactersFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVFindFontCharactersFactory( boost::bind( &SVMatroxOcrInterface::FindFontCharacters, _1, _2 ) ) ) )
	( SVLoadFontFactory::GetStaticCommandType(),           SVCommandFactoryTemplatePtr( new SVLoadFontFactory( boost::bind( &SVMatroxOcrInterface::RestoreFont, _1, _2 ) )                  ) )
	( SVSaveFontFactory::GetStaticCommandType(),           SVCommandFactoryTemplatePtr( new SVSaveFontFactory( boost::bind( &SVMatroxOcrInterface::SaveFont, _1, _2 ) )                     ) )
	( SVCalibrateFontFactory::GetStaticCommandType(),      SVCommandFactoryTemplatePtr( new SVCalibrateFontFactory( boost::bind( &SVMatroxOcrInterface::CalibrateFontCommand, _1, _2 ) )    ) )
	( SVReadFontFactory::GetStaticCommandType(),           SVCommandFactoryTemplatePtr( new SVReadFontFactory( boost::bind( &SVMatroxOcrInterface::ReadString, _1, _2 ) )                   ) )
	( SVVerifyFontFactory::GetStaticCommandType(),         SVCommandFactoryTemplatePtr( new SVVerifyFontFactory( boost::bind( &SVMatroxOcrInterface::VerifyString, _1, _2 ) )               ) )
	( SVCharacterThicknessFactory::GetStaticCommandType(), SVCommandFactoryTemplatePtr( new SVCharacterThicknessFactory( boost::bind( &SVMatroxOcrInterface::CharacterThickness, _1, _2 ) ) ) );

SVMatroxCommandFactory::SVMatroxCommandFactory()
: SVCommandAbstractFactoryTemplate( m_DefaultFactories )
{
}

SVMatroxCommandFactory::~SVMatroxCommandFactory()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxCommandFactory.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Mar 2011 13:36:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to handle new ocr commands from remote systems via SIAC.
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
