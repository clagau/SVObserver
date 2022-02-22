// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma region Includes
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SaxExtractPropertiesHandler.h"
#include  "SaxXMLHandler.h"
#pragma endregion Includes

namespace  SvXml
{
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
				MsgCont.setMessage( SVMSG_SVO_97_CONFIGURATION_TOO_OLD, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16079_ConfigurationTooOld );
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

} //namespace SvXml
