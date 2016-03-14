//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageToLineProject
//* .File Name       : $Workfile:   SVImageToLineProject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 12:44:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageToLineProject.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVImageToLineProjectClass, SVImageToLineProjectClassGuid )

SVImageToLineProjectClass::SVImageToLineProjectClass( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageToLineProjectObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ImageToLineProjectImage" ) );

	inputProjectAngle.SetInputObjectType( SVProjectAngleObjectGuid, SVDoubleValueObjectType );
	inputProjectAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputProjectAngle, _T( "ImageToLineProjectAngle" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputLineObject, SVOutputLineObjectGuid, IDS_OBJECTNAME_LINE1 );

	RegisterEmbeddedObject( &thresholdMin, SVThresholdMinObjectGuid, IDS_OBJECTNAME_THRESHOLD_MINVALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &thresholdMax, SVThresholdMaxObjectGuid, IDS_OBJECTNAME_THRESHOLD_MAXVALUE, false, SVResetItemNone );

	// Set Embedded defaults
	thresholdMin.SetDefaultValue( 0,TRUE );
	thresholdMax.SetDefaultValue( 0,TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVImageToLineProjectClass::~SVImageToLineProjectClass()
{
	CloseObject();
}

BOOL SVImageToLineProjectClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	bOk &= getInputImage() != NULL;

	if( bOk )
	{
		SVImageInfoClass ImageInfo = getInputImage()->GetImageInfo();

		if( ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, outputLineObject.m_lPixelDepth ) == S_OK )
		{
			// Set range...
			outputLineObject.m_dwLineArrayDataType = SV_DWORD;
			outputLineObject.m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( outputLineObject.m_lPixelDepth) );	
			outputLineObject.m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( outputLineObject.m_lPixelDepth) );	
			outputLineObject.m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( outputLineObject.m_lPixelDepth) );
			outputLineObject.m_svNormalizer.SetNormalRange( SVGetDataTypeMin( outputLineObject.m_lPixelDepth ), SVGetDataTypeMax( outputLineObject.m_lPixelDepth ) );
			outputLineObject.m_svNormalizer.SetRealRange( outputLineObject.m_dwMinThreshold, outputLineObject.m_dwMaxThreshold );

			// Create Project data buffer...
			SVDataBufferInfoClass &rBufferInfo = dataBuffer.GetDataBufferInfo();
			rBufferInfo.Type = SVDataBufferInfoClass::SVProjectResult;
		}
	}

	bOk &= UpdateLineExtentData() == S_OK;

	// Reset Printable Flag
	thresholdMin.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	thresholdMax.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVImageToLineProjectClass::CloseObject()
{
	if( SVTaskObjectClass::CloseObject() && getOutputLine() )
	{
		BOOL bRetVal = dataBuffer.CloseObject();
		return getOutputLine()->CloseObject() && bRetVal;
	}
	return FALSE;
}

HRESULT SVImageToLineProjectClass::ResetObject()
{
	HRESULT	l_hrOk = UpdateLineExtentData();

	if( l_hrOk == S_OK )
	{
		if ( ! dataBuffer.Resize( outputLineObject.mlLinePixelLength ) )
		{
			l_hrOk = S_FALSE;
		}
	}

	if( SVTaskObjectClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVImageClass* SVImageToLineProjectClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVImageToLineProjectClass::getInputProjectAngle()
{
	if( inputProjectAngle.IsConnected() && inputProjectAngle.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputProjectAngle.GetInputObjectInfo().PObject;

	return NULL;
}

SVLineClass* SVImageToLineProjectClass::getOutputLine()
{
	return &outputLineObject;
}

BOOL SVImageToLineProjectClass::onRun( SVRunStatusClass& RRunStatus )
{

	DWORD maxValue = 0;
	DWORD minValue = 0;

	// All inputs and outputs must be validated first
	if( SVTaskObjectClass::onRun( RRunStatus ) )
	{
		thresholdMax.SetValue( RRunStatus.m_lResultDataIndex, 0 );
		thresholdMin.SetValue( RRunStatus.m_lResultDataIndex, 0 );

		SVImageClass* pInputImage = getInputImage();
		ASSERT( pInputImage );
		SVDoubleValueObjectClass* pProjAngle = getInputProjectAngle();
		ASSERT( pProjAngle );
		SVLineClass* pOutputLine  = getOutputLine();
		ASSERT( pOutputLine );

		double projAngle;
		if( S_OK != pProjAngle->GetValue( projAngle ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();
		SVSmartHandlePointer ImageHandle;

		if ( ! pInputImage->GetImageHandle( ImageHandle )  || ImageHandle.empty() )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		SVImageBufferHandleImage l_MilHandle;
		if( ImageHandle->GetData( l_MilHandle ) != S_OK || l_MilHandle.empty() )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		SVDataBufferInfoClass& rDataBufferInfo = dataBuffer.GetDataBufferInfo();

		double l_dHeight;
		SVImageExtentClass l_svExtents = pInputImage->GetImageExtents();
		l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight );

		SVMatroxImageInterface::SVStatusCode l_Code;

		// Calc projection 2D -> 1D...
		l_Code = SVMatroxImageInterface::Project( rDataBufferInfo.HBuffer.milResult, l_MilHandle.GetBuffer(), projAngle );

		l_Code = SVMatroxImageInterface::GetResult( rDataBufferInfo.HBuffer.milResult, pOutputLine->GetDataBuffer());

		// Read profile...
		if( pOutputLine->ReadData( l_dHeight ) )
		{
			double divisor = pOutputLine->m_dwColorNumber;

			// Normalize line values...
			if( divisor != 0.0 )
			{
				for( int i = 0; i < pOutputLine->mlLineArrayLength; ++ i )
				{
					double dPixel;

					// Get Pixel of Line...
					pOutputLine->mNormalizedArray.GetAt( i, (void *)(&dPixel) );

					DWORD dwValue = (DWORD)(dPixel);

					// Normalize to ColorNumber...
					if( l_dHeight > 1.0 )
					{
						// calculate Min and Max
						if( !i )
						{
							minValue = dwValue;
							maxValue = dwValue;
						}
						else
						{
							minValue = std::min( minValue, dwValue );
							maxValue = std::max( maxValue, dwValue );
						}
					}
				}
			}

			// Set Min/max values
			thresholdMax.SetValue( RRunStatus.m_lResultDataIndex, maxValue );
			thresholdMin.SetValue( RRunStatus.m_lResultDataIndex, minValue );

			return TRUE;
		}

		// Something is wrong...
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	return FALSE;
}

BOOL SVImageToLineProjectClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( inputImageObjectInfo.IsConnected() &&
		inputImageObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}


DWORD_PTR SVImageToLineProjectClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
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

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

HRESULT SVImageToLineProjectClass::GetHistogram( SVExtentMultiLineStruct& p_rsvMiltiLine )
{
	HRESULT l_hr = S_OK;

	SVImageClass* pInputImage = getInputImage();
	ASSERT( pInputImage );
	SVDoubleValueObjectClass* pProjAngle = getInputProjectAngle();
	ASSERT( pProjAngle );
	SVLineClass* pOutputLine  = getOutputLine();
	ASSERT( pOutputLine );

	if( pOutputLine->m_bDisplayGraph )
	{
		double projAngle;
		if( S_OK != pProjAngle->GetValue( projAngle ) )
		{
			l_hr =  S_FALSE;
		}

		double l_dHeight = 1.0;

		SVImageExtentClass l_svExtents = pInputImage->GetImageExtents();
		l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_dHeight );

		// Read profile...
		if( l_hr == S_OK && pOutputLine->ReadData( l_dHeight ) )
		{
			double divisor = pOutputLine->m_dwColorNumber;

			if( divisor != 0.0 )
			{
				SVExtentLineStruct l_svHistoGramFigure;

				DWORD dwPixel;
				SVExtentPointStruct graphPoint;

				// Calculate Graph...
				RECT l_oRect;


				SVExtentPointStruct l_svPoint;

				l_svExtents.GetOutputRectangle( l_oRect );

				l_svHistoGramFigure.Initialize(); // SetListSize( pOutputLine->mlLineArrayLength );
				l_svHistoGramFigure.m_svPointArray.SetAtGrow(pOutputLine->mlLineArrayLength - 1 , SVExtentPointStruct(0,0));
				for( int i = 0; i < pOutputLine->mlLineArrayLength; ++ i )
				{
					double dPixel;

					// Get Pixel of Line...
					pOutputLine->mLineArray.GetAt( i, (void *)(&dwPixel) );
					pOutputLine->mNormalizedArray.GetAt( i, (void *)(&dPixel) );

					DWORD dwValue = (DWORD)(dPixel);


					// Must be weighted by color number!
					graphPoint.m_dPositionX = i;
					graphPoint.m_dPositionY =  ( long ) ( ( ( double ) dwPixel ) / divisor );

					if( projAngle == 0.0 )
					{
						graphPoint.m_dPositionY = l_oRect.bottom - graphPoint.m_dPositionY;
					}
					else if( projAngle == 90.0 )
					{
						// Vertical profile type
						long swapValue = static_cast<long>(graphPoint.m_dPositionX);

						graphPoint.m_dPositionX = l_oRect.right - graphPoint.m_dPositionY;
						graphPoint.m_dPositionY = swapValue;
					}

					l_svHistoGramFigure.m_svPointArray.SetAtGrow( i, graphPoint );
				}

				l_svHistoGramFigure.m_dwColor = SV_DEFAULT_SUB_FUNCTION_COLOR_1;

				p_rsvMiltiLine.m_svLineArray.Add( l_svHistoGramFigure );
			}
			else
			{
				l_hr = S_FALSE;
			}
		}
	}

	return l_hr;
}

HRESULT SVImageToLineProjectClass::GetThresholdBars( SVExtentMultiLineStruct& p_rsvMiltiLine )
{
	HRESULT l_hr = S_OK;

	SVImageClass* pInputImage = getInputImage();
	ASSERT( pInputImage );
	SVLineClass* pOutputLine  = getOutputLine();
	ASSERT( pOutputLine );

	if( pInputImage && pOutputLine && pOutputLine->m_bDisplayGraph )
	{
		double divisor = pOutputLine->m_dwColorNumber;
		SVImageExtentClass l_svExtents = pInputImage->GetImageExtents();

		RECT l_oRect;

		l_svExtents.GetOutputRectangle( l_oRect );


		// Calc current Threshold bars...
		long lMaxThresholdValue = ( long ) ( pOutputLine->m_dwDisplayedMaxThresholdBarValue / divisor * ( ( double ) l_oRect.bottom ) );
		long lMinThresholdValue = ( long ) ( pOutputLine->m_dwDisplayedMinThresholdBarValue / divisor * ( ( double ) l_oRect.bottom ) );

		// MaxThresholdBar
		// Max left...
		SVExtentPointStruct graphPoint;
		graphPoint.m_dPositionX = 0;
		graphPoint.m_dPositionY = l_oRect.bottom - lMaxThresholdValue;

		SVExtentLineStruct l_svExtentLine;

		l_svExtentLine.m_bIsAngleValid = true;
		l_svExtentLine.m_dAngle = 0.0;

		l_svExtentLine.m_svPointArray.SetAtGrow( 0, graphPoint);

		// Max right...
		graphPoint.m_dPositionX = l_oRect.right;

		l_svExtentLine.m_svPointArray.SetAtGrow( 1, graphPoint);

		l_svExtentLine.m_dwColor = SV_DEFAULT_SUB_FUNCTION_COLOR_1;

		p_rsvMiltiLine.m_svLineArray.Add( l_svExtentLine );

		// MinThresholdBar
		// Min left...
		graphPoint.m_dPositionX = 0;
		graphPoint.m_dPositionY = l_oRect.bottom - lMinThresholdValue;

		l_svExtentLine.m_svPointArray.SetAtGrow( 0, graphPoint );

		// Min right...
		graphPoint.m_dPositionX = l_oRect.right ;

		l_svExtentLine.m_svPointArray.SetAtGrow( 1, graphPoint );

		l_svExtentLine.m_dwColor = SV_DEFAULT_SUB_FUNCTION_COLOR_1;

		p_rsvMiltiLine.m_svLineArray.Add( l_svExtentLine );
	}

	return l_hr;
}

HRESULT SVImageToLineProjectClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT l_Status = S_OK;

	SVExtentFigureStruct l_svFigure;
	SVExtentMultiLineStruct l_svMultiLine;

	SVImageExtentClass l_svToolExtents;
	BOOL l_bOk = TRUE;

	l_bOk = l_bOk && GetTool() != NULL && GetTool()->GetImageExtent( l_svToolExtents ) == S_OK;

	SVExtentLineStruct l_svLine;

	if ( GetHistogram( l_svMultiLine ) == S_OK )
	{
		l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
		p_MultiLineArray.Add(l_svMultiLine);
		l_svMultiLine.Initialize();
	}

	if ( GetThresholdBars( l_svMultiLine ) == S_OK )
	{
		l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
		p_MultiLineArray.Add(l_svMultiLine);
		l_svMultiLine.Initialize();
	}

	return l_Status;
}

HRESULT SVImageToLineProjectClass::UpdateLineExtentData()
{
	HRESULT l_hrOk = S_OK;

	SVImageExtentClass l_svExtents;

	SVDoubleValueObjectClass* pProjAngle = getInputProjectAngle();

	double projAngle = 0.0;

	RECT l_oRect;

	// This is the new Absolute Extent of the Image
	if ( GetTool() != NULL && pProjAngle != NULL &&
		GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
		l_svExtents.GetOutputRectangle( l_oRect ) == S_OK &&
		( pProjAngle->GetValue( projAngle ) == S_OK ) )
	{
		POINT l_oStart;
		POINT l_oEnd;

		long l_lLength = 0;

		if( projAngle == 0.0 )
		{
			l_lLength = l_oRect.right;

			l_oStart.x = 0;
			l_oStart.y = l_oRect.bottom / 2;

			l_oEnd.x = l_oRect.right;
			l_oEnd.y = l_oRect.bottom / 2;
		}
		else if( projAngle == 90.0 )
		{
			l_lLength = l_oRect.bottom;

			l_oStart.x = l_oRect.right / 2;
			l_oStart.y = 0;

			l_oEnd.x = l_oRect.right / 2;
			l_oEnd.y = l_oRect.bottom;
		}

		outputLineObject.m_svLineExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_oStart );
		outputLineObject.m_svLineExtent.SetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_oEnd );
		outputLineObject.m_svLineExtent.UpdateData();

		double l_dWidth = l_oEnd.x - l_oStart.x;
		double l_dHeight = l_oEnd.y - l_oStart.y;

		outputLineObject.mlLinePixelLength = SVCalcLinePixelNumber( static_cast<long>(l_dWidth), static_cast<long>(l_dHeight) );
	}

	return l_hrOk;
}

