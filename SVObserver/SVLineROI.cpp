//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineROI
//* .File Name       : $Workfile:   SVLineROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 13:12:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVLineROI.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVImageClass.h"
#include "SVLineAnalyzer.h"
#include "SVTool.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

SV_IMPLEMENT_CLASS( SVLineROIClass, SVLineROIClassGuid )

SVLineROIClass::SVLineROIClass( SVObjectClass* POwner, int StringResourceID )
			   :SVROIClass( POwner, StringResourceID ) 
{

	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVROIObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVLineROIObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputLineObject, SVOutputLineObjectGuid, IDS_OBJECTNAME_LINE1 );

	// Set Embedded defaults

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVLineROIClass::~SVLineROIClass()
{
	CloseObject();
}

BOOL SVLineROIClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	SVImageInfoClass ImageInfo;
    
	BOOL l_bOk = SVROIClass::CreateObject( PCreateStructure );
	
	l_bOk &= getInputImage() != NULL;
		
	if( l_bOk )
	{
		ImageInfo = getInputImage()->GetImageInfo();
	}

	SVLineAnalyzerClass *l_psvAnalyzer = dynamic_cast<SVLineAnalyzerClass *>(GetAnalyzer());

	l_bOk &= l_psvAnalyzer != NULL && l_psvAnalyzer->GetImageExtent( outputLineObject.m_svLineExtent ) == S_OK;

	if ( l_bOk )
	{
		ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, outputLineObject.m_lPixelDepth );

		outputLineObject.m_dwLineArrayDataType = outputLineObject.m_lPixelDepth;
		outputLineObject.m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( outputLineObject.m_lPixelDepth ));
		outputLineObject.m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( outputLineObject.m_lPixelDepth ));
		outputLineObject.m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( outputLineObject.m_lPixelDepth ));
		outputLineObject.m_svNormalizer.SetRealRange( outputLineObject.m_dwMinThreshold, outputLineObject.m_dwMaxThreshold );
	}

	// Set / Reset Printable Flag
	outputLineObject.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = l_bOk;

	return l_bOk;
}

BOOL SVLineROIClass::CloseObject()
{
	return SVROIClass::CloseObject();
}

HRESULT SVLineROIClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	SVLineAnalyzerClass *l_psvAnalyzer = dynamic_cast<SVLineAnalyzerClass *>(GetAnalyzer());
	SVImageClass *l_psvInputImage = getInputImage();

	if ( l_psvAnalyzer != NULL && l_psvInputImage != NULL &&
	     l_psvAnalyzer->GetImageExtent( outputLineObject.m_svLineExtent ) == S_OK )
	{
		l_hrOk = l_psvInputImage->ValidateAgainstOutputExtents( outputLineObject.m_svLineExtent );

		if( l_hrOk != S_OK )
		{
			RECT l_oRect;

			POINT l_oPoint1;
			POINT l_oPoint2;

			SVImageExtentClass l_svExtent = l_psvInputImage->GetImageExtents();

			if( l_svExtent.GetOutputRectangle( l_oRect ) == S_OK &&
			    outputLineObject.m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint1 ) == S_OK &&
				  outputLineObject.m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_oPoint2 ) == S_OK )
			{
				long l_lMaxX = std::max( l_oPoint1.x, l_oPoint2.x );
				long l_lMaxY = std::max( l_oPoint1.y, l_oPoint2.y );

				if( l_oRect.right - 1 < l_lMaxX )
				{
					l_oPoint1.x -= l_lMaxX - ( l_oRect.right - 1 );
					l_oPoint2.x -= l_lMaxX - ( l_oRect.right - 1 );
				}

				if( l_oPoint1.x < 0 )
				{
					l_oPoint1.x = 0;
				}

				if( l_oPoint2.x < 0 )
				{
					l_oPoint2.x = 0;
				}

				if( l_oRect.bottom - 1 < l_lMaxY )
				{
					l_oPoint1.y -= l_lMaxY - ( l_oRect.bottom - 1 );
					l_oPoint2.y -= l_lMaxY - ( l_oRect.bottom - 1 );
				}

				if( l_oPoint1.y < 0 )
				{
					l_oPoint1.y = 0;
				}

				if( l_oPoint2.y < 0 )
				{
					l_oPoint2.y = 0;
				}

				if( outputLineObject.m_svLineExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_oPoint1 ) == S_OK &&
				    outputLineObject.m_svLineExtent.SetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_oPoint2 ) == S_OK &&
						outputLineObject.m_svLineExtent.UpdateData() == S_OK )
				{
					l_hrOk = l_psvAnalyzer->SetImageExtent( 1, outputLineObject.m_svLineExtent );
				}
      }
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	if( SVROIClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVLineClass* SVLineROIClass::getOutputLine()
{
	return &outputLineObject;
}

BOOL SVLineROIClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVROIClass::onRun( RRunStatus );
	// All inputs and outputs must be validated first
	if( l_bOk )
	{
		SVLineClass *l_pLine  = getOutputLine();
		SVImageClass *l_pImage = getInputImage();
		
		l_bOk = l_pImage != NULL && l_pLine != NULL;
		
		if( l_bOk )
		{
			SVSmartHandlePointer ImageHandle;
			
			POINT l_oHeadPoint;
			POINT l_oTailPoint;
			
			l_bOk = l_pImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() );
			l_bOk = l_bOk && l_pImage->ValidateAgainstOutputExtents( outputLineObject.m_svLineExtent ) == S_OK;
			l_bOk = l_bOk && l_pLine->GetHeadPoint( l_oHeadPoint ) == S_OK;
			l_bOk = l_bOk && l_pLine->GetTailPoint( l_oTailPoint ) == S_OK;
			
			if( l_bOk )
			{
				SVMatroxBufferInterface::SVStatusCode l_Code;
				
				SVImageBufferHandleImage l_MilHandle;
				ImageHandle->GetData( l_MilHandle );

				SVMatroxIdentifier l_ArrayLength = 0;

				l_Code = SVMatroxBufferInterface::GetLine( l_MilHandle.GetBuffer(), 
					l_oHeadPoint.x,
					l_oHeadPoint.y, 
					l_oTailPoint.x - 1, 
					l_oTailPoint.y - 1, 
					l_ArrayLength, 
					l_pLine->GetDataBuffer() );

				l_pLine->mlLineArrayLength = static_cast< long >( l_ArrayLength );
				
				l_bOk = l_bOk && l_pLine->ReadLine();
			}
		}
		
		if( ! l_bOk )
		{
			// Something is wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
		}
	}
	
	return l_bOk;
}

DWORD_PTR SVLineROIClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return (DwResult | SVROIClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

