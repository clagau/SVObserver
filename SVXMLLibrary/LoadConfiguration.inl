// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma once

#pragma region Includes

#include "ObjectInterfaces\ErrorNumbers.h"
#include  "SaxXMLHandler.h"
#pragma endregion Includes
#include "SaxExtractPropertiesHandler.h"



template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration(unsigned long& ulSVOConfigVersion, LPCTSTR FileName, SVTreeType& p_rTree)
{


	SvXml::SaxExtractPropertiesHandler   SaxExtractHandler;
	HRESULT hrOK = SaxExtractHandler.ExtractProperties(FileName, ulSVOConfigVersion);

	if( SUCCEEDED( hrOK ) )
	{

		if(ulSVOConfigVersion < 0x00043200)
		{

			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_SVO_97_CONFIGURATION_TOO_OLD, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16079_ConfigurationTooOld );
			throw MsgCont;	
		}
		else  
		{
			SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
			hrOK = SaxHandler.BuildFromXMLFile(&p_rTree, FileName);
		}


	}
	return hrOK;
}

