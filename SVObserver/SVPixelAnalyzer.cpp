//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PixelAnalyzer
//* .File Name       : $Workfile:   SVPixelAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPixelAnalyzer.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVOCore/SVDataBuffer.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVPixelAnalyzerSetup.h" // Required by SVPixelAnalyzerSetupClass
#include "SVResultLong.h"   // Required by SVLongResultClass
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVPixelAnalyzerClass, SVPixelAnalyzerClassGuid );

SVPixelAnalyzerClass::SVPixelAnalyzerClass( LPCSTR ObjectName )
: SVImageAnalyzerClass( ObjectName )
{
	init();
}

SVPixelAnalyzerClass::SVPixelAnalyzerClass(BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVPixelAnalyzerClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVPixelAnalyzerObjectType;

	// Identify our input type needs

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_pixelCount, SVPixelCountObjectGuid, IDS_OBJECTNAME_PIXEL_COUNT, false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(&m_pixelCountColor, SVPixelColorIndexObjectGuid,	IDS_OBJECTNAME_PIXEL_COLOR_INDEX, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_pixelCountColor. SetDefaultValue( 255, true ); // White
	m_pixelCount. SetDefaultValue( 0, true );
	m_pixelCount.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();

	// Instantiate Children
	SVLongResultClass *pAnalyzerResult = new SVLongResultClass (true, this, IDS_CLASSNAME_SVPIXELANALYZERESULT);

	if (!pAnalyzerResult)
	{
		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16187);

	}

	Add( pAnalyzerResult );

	// set defaults
	m_alHistValues.clear();

}

SVPixelAnalyzerClass::~SVPixelAnalyzerClass()
{
	CloseObject();
}

BOOL SVPixelAnalyzerClass::CloseObject()
{
    m_alHistValues.clear();

	if ( !m_histResultID.empty() )
	{
		HRESULT l_Code;
		l_Code = SVMatroxImageInterface::Destroy( m_histResultID );
	}
	return SVImageAnalyzerClass::CloseObject();
}

BOOL SVPixelAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
    SVImageClass *pSVImage(nullptr);
	DWORD LastError(0);
    	
    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
			
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16121);
			LastError =  -SvStl::Err_16121;
			break;
        }
		
        pSVImage = getInputImage ();
		
        if (!pSVImage)
        {
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16122);
			LastError =  -SvStl::Err_16122;
			break;
        }
		try
		{
			m_svlHistValueArraySize = 1 << (GetInputPixelDepth () & SVBufferSize);
			m_alHistValues.resize( m_svlHistValueArraySize );
		}
		catch(const SvStl::MessageContainer& rContain)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( rContain.getMessage());
			LastError =  -SvStl::Err_16123;
			break;
		}
        for( int i = 0; i < m_svlHistValueArraySize; i++ )
		{
			m_alHistValues [i] = 0L;
		}
        // &&&
		SVDataBufferInfoClass svData;
		
		svData.Length = m_svlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_histResultID;
		if ( S_OK == SVImageProcessingClass::CreateDataBuffer( &svData ) )
		{
			m_histResultID = svData.HBuffer.milResult;
		}
		
		if( m_histResultID.empty() )
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16124);
			LastError =  -SvStl::Err_16124;
			break;
        }
        break;
    }
	
    if (0 != LastError)
    {
        m_isCreated = false;
    }
    else
	{
        m_isCreated = true;
	}
	
	// Set / Reset Printable Flags
	m_pixelCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_pixelCountColor.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_pixelCountColor.SetOutputFormat(OutputFormat_int);
	
	return m_isCreated;
}

bool SVPixelAnalyzerClass::onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	SVImageClass* pInputImage (nullptr);
	BYTE byIndex(0);
	bool Result = true;

	while (1)
	{
		SVSmartHandlePointer ImageHandle;

		pInputImage = getInputImage ();

		if( ! pInputImage )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16188, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		if ( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16189, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		HRESULT MatroxCode = SVMatroxImageInterface::Histogram( m_histResultID, l_MilHandle.GetBuffer() );

		if (S_OK != MatroxCode)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16190, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		MatroxCode = SVMatroxImageInterface::GetResult( m_histResultID, m_alHistValues );

		if (S_OK != MatroxCode)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16191, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			break;
		}

		m_pixelCountColor.GetValue(byIndex);
		m_pixelCount.SetValue(m_alHistValues[byIndex]);

		break;
	}

	if (!Result)
	{
		SetInvalid ();
		rRunStatus.SetInvalid ();
	}
	return Result;
}

bool SVPixelAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Valid = __super::ResetObject(pErrorMessages);

	if (m_histResultID.empty())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16197, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	if (m_alHistValues.size() != m_svlHistValueArraySize)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(  SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16198, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Valid = false;
	}

	return Valid;
}
