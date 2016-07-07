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

#include "SVDataBuffer.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVPixelAnalyzerSetup.h" // Required by SVPixelAnalyzerSetupClass
#include "SVResultLong.h"   // Required by SVLongResultClass
#include "SVOMFCLibrary/SVDeviceParams.h"
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
    while (1)
    {
		// Identify our output type
	    m_outObjectInfo.ObjectTypeInfo.SubType = SVPixelAnalyzerObjectType;

	    // Identify our input type needs

	    // Register Embedded Objects
		RegisterEmbeddedObject(&m_pixelCount, SVPixelCountObjectGuid, IDS_OBJECTNAME_PIXEL_COUNT, false, SVResetItemNone);
			
			RegisterEmbeddedObject(&m_pixelCountColor, SVPixelColorIndexObjectGuid,	IDS_OBJECTNAME_PIXEL_COLOR_INDEX, false, SVResetItemNone);
			
	    // Set Embedded defaults
	    m_pixelCountColor. SetDefaultValue( 255, true ); // White
	    m_pixelCount. SetDefaultValue( 0, true );

	    // Set default inputs and outputs
	    addDefaultInputObjects();
	    
	    // Instantiate Children
        SVLongResultClass *pAnalyzerResult = new SVLongResultClass (true, this, IDS_CLASSNAME_SVPIXELANALYZERESULT);

	    if (!pAnalyzerResult)
	    {
            msvError.msvlErrorCd = -1082;
            SV_TRAP_ERROR (msvError, 1082);
	    }

        Add( pAnalyzerResult );

	    // set defaults
        m_alHistValues.clear();
        break;
    }
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
		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Destroy( m_histResultID );
	}
	return SVImageAnalyzerClass::CloseObject();
}

BOOL SVPixelAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
    SVImageClass *pSVImage;
	
    msvError.ClearLastErrorCd ();
	
    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
			msvError.msvlErrorCd = -1084;
			SV_TRAP_ERROR_BRK (msvError, 1084);
        }
		
        pSVImage = getInputImage ();
		
        if (!pSVImage)
        {
			msvError.msvlErrorCd = -1085;
			SV_TRAP_ERROR_BRK (msvError, 1085);
        }
		
        m_svlHistValueArraySize = 1 << (GetInputPixelDepth () & SVBufferSize);
		
        m_alHistValues.resize( m_svlHistValueArraySize );
		
        for( int i = 0; i < m_svlHistValueArraySize; i++ )
		{
			m_alHistValues [i] = 0L;
		}
        // &&&
		SVDataBufferInfoClass svData;
		
		svData.Length = m_svlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = m_histResultID;
		if ( S_OK == SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) )
		{
			m_histResultID = svData.HBuffer.milResult;
		}
		
		if( m_histResultID.empty() )
		{
			msvError.msvlErrorCd = -1087;
			SV_TRAP_ERROR_BRK (msvError, 1087);
        }
        break;
    }
	
    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
        m_isCreated = false;
    }
    else
	{
        m_isCreated = true;
	}
	
	// Set / Reset Printable Flags
	m_pixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_pixelCountColor.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	
    return m_isCreated;
}
BOOL SVPixelAnalyzerClass::onRun(SVRunStatusClass &RRunStatus)
{
    SVImageClass* pInputImage (nullptr);
    BYTE byIndex;

    msvError.ClearLastErrorCd ();

    while (1)
    {
		SVSmartHandlePointer ImageHandle;

		SVMatroxImageInterface::SVStatusCode l_Code;

        pInputImage = getInputImage ();

        if( ! pInputImage )
        {
            msvError.msvlErrorCd = -1089;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1089)
        }

        if ( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
        {
            msvError.msvlErrorCd = -1089;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1089)
        }

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		l_Code = SVMatroxImageInterface::Histogram( m_histResultID, l_MilHandle.GetBuffer() );

		if( l_Code != SVMEE_STATUS_OK )
        {
			//          35 = Invalid MIL ID, for others see milerr.h
            msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1090)
        }
		
		l_Code = SVMatroxImageInterface::GetResult( m_histResultID, m_alHistValues );
		
		if( l_Code != SVMEE_STATUS_OK )
        {
            msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1091)
        }
		
		m_pixelCountColor.GetValue (byIndex);
		m_pixelCount.SetValue( RRunStatus.m_lResultDataIndex, m_alHistValues[byIndex] );
		
        break;
    }
	
    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
		SetInvalid ();            
		RRunStatus.SetInvalid ();
		
		return false;
    }
    return true;
}

BOOL SVPixelAnalyzerClass::OnValidate ()
{
	msvError.ClearLastErrorCd ();
	msvError.msvlErrorCd = 0x00000000;

	while (1)
	{
		if (!SVImageAnalyzerClass::OnValidate ())
		{
			break;
		}

		if ( m_histResultID.empty() )
		{
			msvError.msvlErrorCd = -1093;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1093);
		}

		if ( m_alHistValues.size() != m_svlHistValueArraySize)
		{
			msvError.msvlErrorCd = -1094;
			SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1094);
		}
		break;
	}

	if( (msvError.GetLastErrorCd () & SV_ERROR_CONDITION) ||
		(msvError.msvlErrorCd       & SV_ERROR_CONDITION) )
	{
		SetInvalid();
		return false;
	}
	isObjectValid.SetValue(1, true);
	return true;
}

