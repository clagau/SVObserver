//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRotateReference
//* .File Name       : $Workfile:   SVRotateReference.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 12:37:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVRotateReference.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SV_IMPLEMENT_CLASS( SVRotateReferenceClass, SVRotateReferenceClassGuid )

SVRotateReferenceClass::SVRotateReferenceClass( SVObjectClass* POwner, int StringResourceID )
					   :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVRotateReferenceObjectType;

	// Identify our input type needs

	// X
	inputXEnabled.SetInputObjectType( SVOutputEvaluateXEnabledObjectGuid, SVBoolValueObjectType );
	inputXEnabled.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputXEnabled, _T( "RotateReferenceXEnable" ) );

	inputXResult.SetInputObjectType( SVOutputEvaluateXResultObjectGuid, SVDoubleValueObjectType );
	inputXResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputXResult, _T( "RotateReferenceXResult" ) );

	// Y
	inputYEnabled.SetInputObjectType( SVOutputEvaluateYEnabledObjectGuid, SVBoolValueObjectType );
	inputYEnabled.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputYEnabled, _T( "RotateReferenceYEnable" ) );

	inputYResult.SetInputObjectType( SVOutputEvaluateYResultObjectGuid, SVDoubleValueObjectType );
	inputYResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputYResult, _T( "RotateReferenceYResult" ) );

	// Beta
	inputBetaEnabled.SetInputObjectType( SVOutputEvaluateBetaEnabledObjectGuid, SVBoolValueObjectType );
	inputBetaEnabled.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputBetaEnabled, _T( "RotateReferenceBetaEnable" ) );

	inputBetaResult.SetInputObjectType( SVOutputEvaluateBetaResultObjectGuid, SVDoubleValueObjectType );
	inputBetaResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputBetaResult, _T( "RotateReferenceBetaResult" ) );

	// Image
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "RotateReferenceImage" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &learnedX, SVOutputLearnedXObjectGuid, IDS_OBJECTNAME_LEARNED_X, false, SVResetItemNone );
	RegisterEmbeddedObject( &learnedY, SVOutputLearnedYObjectGuid, IDS_OBJECTNAME_LEARNED_Y, false, SVResetItemNone );
	RegisterEmbeddedObject( &learnedBeta, SVOutputLearnedBetaObjectGuid, IDS_OBJECTNAME_LEARNED_BETA, false, SVResetItemNone );

	// Set Embedded defaults
	learnedX.SetDefaultValue( 0, TRUE );
	learnedY.SetDefaultValue( 0, TRUE );
	learnedBeta.SetDefaultValue( 0, TRUE );

	outputImageObject.InitializeImage( SVImageTypePhysical );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVRotateReferenceClass::~SVRotateReferenceClass()
{

}

BOOL SVRotateReferenceClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	bOk &= ( outputImageObject.InitializeImage( getInputImage() ) == S_OK ); 

	// Set / Reset Printable Flags
	learnedBeta.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedX.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	learnedY.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVRotateReferenceClass::CloseObject()
{
	return SVTaskObjectClass::CloseObject();
}

HRESULT SVRotateReferenceClass::ResetObject()
{
	HRESULT l_hrOk = outputImageObject.InitializeImage( getInputImage() );

	if ( SVTaskObjectClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVRotateReferenceClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL && p_psvImage == getInputImage() )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}


DWORD SVRotateReferenceClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
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

SVBoolValueObjectClass* SVRotateReferenceClass::getInputXEnabled()
{
	if( inputXEnabled.IsConnected() && inputXEnabled.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputXEnabled.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getInputXResult()
{
	if( inputXResult.IsConnected() && inputXResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputXResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVBoolValueObjectClass* SVRotateReferenceClass::getInputYEnabled()
{
	if( inputYEnabled.IsConnected() && inputYEnabled.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputYEnabled.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getInputYResult()
{
	if( inputYResult.IsConnected() && inputYResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputYResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVBoolValueObjectClass* SVRotateReferenceClass::getInputBetaEnabled()
{
	if( inputBetaEnabled.IsConnected() && inputBetaEnabled.GetInputObjectInfo().PObject )
		return ( SVBoolValueObjectClass* ) inputBetaEnabled.GetInputObjectInfo().PObject;

	return NULL;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getInputBetaResult()
{
	if( inputBetaResult.IsConnected() && inputBetaResult.GetInputObjectInfo().PObject )
		return ( SVDoubleValueObjectClass* ) inputBetaResult.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVRotateReferenceClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVImageClass* SVRotateReferenceClass::getOutputImage()
{
	return &outputImageObject;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getLearnedX()
{
	return &learnedX;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getLearnedY()
{
	return &learnedY;
}

SVDoubleValueObjectClass* SVRotateReferenceClass::getLearnedBeta()
{
	return &learnedBeta;
}

BOOL SVRotateReferenceClass::onRun( SVRunStatusClass& RRunStatus )
{
	double l_dRotationAngle = 0.0;
	double l_dRotationX = 0.0;
	double l_dRotationY = 0.0;
	double l_dOutputRotationX = 0.0;
	double l_dOutputRotationY = 0.0;

	double l_dResultX = 0.0;
	double l_dResultY = 0.0;

	BOOL l_bXEnabled = FALSE;
	BOOL l_bYEnabled = FALSE;

	BOOL l_bOk = SVTaskObjectClass::onRun( RRunStatus );

	SVImageClass *l_psvInputImage = getInputImage();

	l_bOk = l_bOk && GetTool() != NULL && l_psvInputImage != NULL;

	// All inputs and outputs must be validated first
	if( l_bOk )
	{
		BOOL l_bBetaEnabled = FALSE;

		l_bOk = l_bOk && getInputXEnabled() != NULL && ( getInputXEnabled()->GetValue( l_bXEnabled ) == S_OK );
		if ( l_bOk && l_bXEnabled )
		{
			double l_dLearned = 0.0;

			l_bOk = l_bOk && getInputXResult() != NULL && ( getInputXResult()->GetValue( l_dResultX ) == S_OK );
			l_bOk = l_bOk && ( learnedX.GetValue( l_dLearned ) == S_OK );

			l_dResultX -= l_dLearned;
		}
		else
		{
			l_bOk = l_bOk && ( learnedX.GetValue( l_dResultX ) == S_OK );
		}

		l_bOk = l_bOk && getInputYEnabled() != NULL && ( getInputYEnabled()->GetValue( l_bYEnabled ) == S_OK );
		if ( l_bOk && l_bYEnabled )
		{
			double l_dLearned = 0.0;

			l_bOk = l_bOk && getInputYResult() != NULL && ( getInputYResult()->GetValue( l_dResultY ) == S_OK );
			l_bOk = l_bOk && ( learnedY.GetValue( l_dLearned ) == S_OK );

			l_dResultY -= l_dLearned;
		}
		else
		{
			l_bOk = l_bOk && ( learnedY.GetValue( l_dResultY ) == S_OK );
		}

		l_bOk = l_bOk && getInputBetaEnabled() != NULL && ( getInputBetaEnabled()->GetValue( l_bBetaEnabled ) == S_OK );
		if ( l_bOk && l_bBetaEnabled )
		{
			double l_dResult = 0.0;

			l_bOk = l_bOk && ( learnedBeta.GetValue( l_dRotationAngle ) == S_OK );
			l_bOk = l_bOk && getInputBetaResult() != NULL && ( getInputBetaResult()->GetValue( l_dResult ) == S_OK );

			l_dRotationAngle -= l_dResult;
		}

		if ( l_bOk )
		{
			SVExtentPointStruct l_svInPoint;
			SVExtentPointStruct l_svOutPoint;

			SVImageExtentClass l_svInExtent = l_psvInputImage->GetImageExtents();
			SVImageExtentClass l_svOutExtent = outputImageObject.GetImageExtents();
				
			l_bOk = l_bOk && l_svInExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_svInPoint ) == S_OK;
			l_bOk = l_bOk && l_svOutExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_svOutPoint ) == S_OK;

			if ( l_bOk )
			{
				l_dRotationX = l_dResultX - l_svInPoint.m_dPositionX;
				l_dRotationY = l_dResultY - l_svInPoint.m_dPositionY;
				l_dOutputRotationX = l_dResultX - l_svOutPoint.m_dPositionX;
				l_dOutputRotationY = l_dResultY - l_svOutPoint.m_dPositionY;
			}
		}
	}

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		l_bOk = l_bOk && GetTool()->GetImageExtent( l_svExtents ) == S_OK;

		l_bOk = l_bOk && l_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, l_dRotationAngle ) == S_OK;
		l_bOk = l_bOk && l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointX, l_dRotationX ) == S_OK;
		l_bOk = l_bOk && l_svExtents.SetExtentProperty( SVExtentPropertyPositionPointY, l_dRotationY ) == S_OK;
		l_bOk = l_bOk && l_svExtents.SetExtentProperty( SVExtentPropertyOutputPositionPointX, l_dOutputRotationX ) == S_OK;
		l_bOk = l_bOk && l_svExtents.SetExtentProperty( SVExtentPropertyOutputPositionPointY, l_dOutputRotationY ) == S_OK;

		l_bOk = l_bOk && GetTool()->SetImageExtent( RRunStatus.m_lResultDataIndex, l_svExtents );
	}

	if ( l_bOk )
	{
		SVImageExtentClass l_svExtents;

		l_bOk = GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
			outputImageObject.UpdateImage( l_svExtents ) == S_OK;

		if ( l_bOk )
		{
			SVSmartHandlePointer l_svOutImageHandle;
			SVSmartHandlePointer l_svInImageHandle;

			l_bOk = l_psvInputImage->GetImageHandle( l_svInImageHandle ) && !( l_svInImageHandle.empty() ) &&
							outputImageObject.SetImageHandleIndex( RRunStatus.Images ) &&
							outputImageObject.GetImageHandle( l_svOutImageHandle ) && !( l_svOutImageHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_InMilHandle;
				SVImageBufferHandleImage l_OutMilHandle;

				l_svInImageHandle->GetData( l_InMilHandle );
				l_svOutImageHandle->GetData( l_OutMilHandle );
				
				SVMatroxImageInterface::SVStatusCode l_Code;
				SVMatroxImageRotateStruct l_Rotate( l_InMilHandle.GetBuffer() );
				BOOL l_bEnabled;

				l_Rotate.m_dAngle = l_dRotationAngle;
				l_Rotate.m_dSrcCenX = l_dRotationX;
				l_Rotate.m_dSrcCenY = l_dRotationY;
				l_Rotate.m_dDstCenX = l_dOutputRotationX;
				l_Rotate.m_dDstCenY = l_dOutputRotationY;
				l_Rotate.m_eInterpolation = SVNearestNeighOverScanClear;
				l_Code = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );

				RRunStatus.SetPassed();

				if( getInputXEnabled() != NULL && ( getInputXEnabled()->GetValue( l_bEnabled ) == S_OK ) && l_bEnabled )
				{
					// Store new rotation origin...
					if( S_OK != learnedX.SetValue( RRunStatus.m_lResultDataIndex, l_dResultX ) )
					{
						l_bOk = FALSE;
					}
				}

				if( getInputYEnabled() != NULL && ( getInputYEnabled()->GetValue( l_bEnabled ) == S_OK ) && l_bEnabled )
				{
					// Store new rotation origin...
					if( S_OK != learnedY.SetValue( RRunStatus.m_lResultDataIndex, l_dResultY ) )
					{
						l_bOk = FALSE;
					}
				}
			}

			if ( ! l_bOk )
			{
				RRunStatus.SetFailed();
			}
		}
	}

	if ( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}


BOOL SVRotateReferenceClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( getInputXEnabled()    && 
		getInputXResult()     && 
		getInputYEnabled()    &&
		getInputYResult()     &&
		getInputBetaEnabled() && 
		getInputBetaResult()  && 
		getInputImage()
	  )
	{
		bRetVal = TRUE;
		bRetVal = SVTaskObjectClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRotateReference.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 12:37:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   02 May 2013 11:25:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:01:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   12 Jul 2012 15:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   19 Jun 2012 14:12:30   jspila
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
 *    Rev 3.29   16 Mar 2011 13:51:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   11 Mar 2011 14:31:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   25 Feb 2011 12:22:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   10 Feb 2011 15:11:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   27 Jan 2011 11:54:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   04 Nov 2010 14:11:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   01 Jun 2010 15:08:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   16 Dec 2009 12:34:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   25 Jul 2007 07:48:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   02 Nov 2005 07:33:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   29 Jul 2005 13:22:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   21 Jun 2005 08:32:10   ebeyeler
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
 *    Rev 3.17   17 Feb 2005 15:23:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to implement the new create / reset methodology and new extents.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   16 Feb 2005 15:12:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   22 Apr 2003 15:12:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   31 Jan 2003 11:26:48   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix a check-in problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   30 Jan 2003 16:26:18   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage, onRun, onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   10 Jan 2003 15:54:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   09 Dec 2002 20:04:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   20 Nov 2002 13:14:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 *  Removed references to System
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Apr 2001 19:21:38   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Apr 2001 15:31:22   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   09 Jun 2000 10:11:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  157
 * SCR Title:  Inspection Hangs after loading and going online
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Wrapped TRACE statements in another conditional compile define.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   08 Mar 2000 22:04:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised call to MoveAbsoluteOrigins to pass translation 
 * parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   03 Mar 2000 13:07:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methods to get/set image extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Feb 2000 16:10:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised handler for SVM_GETFIRSTMOVEABLE/RESIZEABLE.
 * Revised onDraw to call pImage->GetTransform() to get contextual transformation.
 * Removed getTranslationContext and getRotationContext
 * methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   21 Feb 2000 14:58:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added getTranslationContext method.
 * Added getRotationContext method.
 * Revised CreateObject() to set display context image.
 * Revised processMessage handling of Move/Size messages
 * to use SVDrawContext
 * Revised onDraw to call checkDrawAllowed().
 * Revised onUpdateFigure to use relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   16 Feb 2000 17:05:10   mike
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
 *    Rev 3.2   08 Feb 2000 10:59:40   mike
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
 *    Rev 3.1   Jan 04 2000 09:39:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  81
 * SCR Title:  Version 3.03 versioning changes
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Some debug code that was left in v3.03 and used in v3.02  but not used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:34:52   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Nov 1999 12:24:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to not allow moving/sizing toolfigure if another
 * object is using the output image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 27 1999 15:14:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix bug in Build Reference which resulted in a Tool failure as a result of a bug in the translation of 'child' images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Oct 1999 14:04:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised onRun to correct walking window problem when
 * x,y,or angle disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 22 1999 14:21:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixed loosing new origin and "fleeing" output window.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:12:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provide rotate (move) capability.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
