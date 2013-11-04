//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageToLineProject
//* .File Name       : $Workfile:   SVImageToLineProject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 12:16:12  $
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


DWORD SVImageToLineProjectClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageToLineProject.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 12:16:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:55:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   01 Aug 2012 12:51:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   27 Jul 2012 08:55:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   25 Jul 2012 14:10:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   12 Jul 2012 14:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   19 Jun 2012 12:47:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   30 Mar 2011 15:08:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   16 Mar 2011 13:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   25 Feb 2011 12:11:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   10 Feb 2011 14:55:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   27 Jan 2011 11:20:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   04 Nov 2010 13:39:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   01 Jun 2010 14:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   16 Dec 2009 11:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   03 Sep 2009 10:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   24 Jul 2007 13:31:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   21 Jun 2007 13:05:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   29 Jul 2005 12:36:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   21 Jun 2005 08:11:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   18 Mar 2005 09:06:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   17 Feb 2005 14:27:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   16 Feb 2005 14:33:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   23 May 2003 08:46:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  342
 * SCR Title:  Configuration report printout changes after going online.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update CreateObject method to fix print flag on Min and Max threshold value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   22 Apr 2003 10:19:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   30 Jan 2003 08:41:34   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject onUpdateFigure, onRun, processMessage, GetContextExtent, isLTResizeable and isRBResizeable methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   14 Jan 2003 13:55:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods SetAllExtents and GetExtent
 * changed the class to inherit from SVExtentHolder instead of SVTaskObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   10 Jan 2003 14:24:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * Added method to return the overlay figure from this object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   09 Dec 2002 19:24:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Nov 2002 13:26:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 *             Changed ValueObject semantics to work with buckets
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Nov 2001 13:23:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update the CreateObject and onRun methods to include sub-pixel resolution to results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Apr 2001 14:38:14   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   26 Apr 2000 18:54:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added isLTResizeable method.
 * Added isRBResizeable method.
 * Added GetTransform method.
 * Added GetGraphFigure method.
 * Added GetThresholdBarsFigure method.
 * Revised CreateObject to retain rotation information.
 * Revised onUpdateFigure method.
 * Revised onRun method.
 * Revised processMessage method to add message handlers
 * for SVMSGID_GETFIRST_LT_RESIZEABLE and
 * SVMSGID_GETFIRST_RB_RESIZEABLE.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   23 Mar 2000 14:05:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  117
 * SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Intorduced/Revised checkDrawAllowed(...) override, to
 * check if tool is allowed to be drawn. 
 * (Necessary for conditional tool drawing)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   13 Mar 2000 16:28:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetImageInputContextGuid to call SVImageClass
 * GetImageInputContextGuid to correct a problem where the
 * edge figures were not being shown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Mar 2000 21:50:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateObject to set extents after setting owner tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   03 Mar 2000 12:30:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLineInfoClass methods to get/set line extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   26 Feb 2000 19:59:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetContextExtent method.
 * Added message handler for SVM_GET_CONTEXT_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   25 Feb 2000 16:05:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onDraw to call pImage->GetTransform() to get contextual transformation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Feb 2000 14:39:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetImageInputContextGuid method.
 * Revised onDraw to call checkDrawAllowed().
 * Revised onUpdateFigure to use relative extents.
 * Revised onRun to use relative extents for graph and
 * threshold figures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Feb 2000 16:57:20   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   08 Feb 2000 10:59:34   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the onDraw function to use the current scaling
 * factor (lZoomFactor) when drawing an object.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Feb 2000 09:41:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to add calculation of minimum and maximum
 * projection values.
 * Revised to expose as outputs the minimum and maximum
 * projection values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 16:40:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  98
 * SCR Title:  Gage Tool Edge Tool Crash
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to protect data array index overrun  access for PLineArray[0].
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:51:20   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Oct 13 1999 15:20:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO. Fixed Normalize Problem in Profile Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 17:47:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New (friend of Line Analyzers if Profile Tool is owner).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
