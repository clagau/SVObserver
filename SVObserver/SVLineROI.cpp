//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineROI
//* .File Name       : $Workfile:   SVLineROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Feb 2014 11:15:32  $
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

LONG_PTR SVLineROIClass::processMessage(DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext)
{
	LONG_PTR DwResult = 0L;

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLineROI.cpp_v  $
 * 
 *    Rev 1.3   01 Feb 2014 11:15:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 14:57:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.34   02 May 2013 11:21:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 12:17:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   25 Jul 2012 14:28:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code and updated overlay functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   18 Jul 2012 13:42:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   02 Jul 2012 17:18:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   30 Mar 2011 15:13:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.40 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use STL min/max functions instead of MFC macros.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   16 Mar 2011 13:40:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   25 Feb 2011 12:17:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   10 Feb 2011 14:58:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   27 Jan 2011 11:28:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   08 Dec 2010 13:02:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   04 Nov 2010 13:43:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   01 Jun 2010 14:52:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   16 Dec 2009 14:03:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   24 Jul 2007 14:27:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update to MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   21 Jun 2007 13:28:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   19 Feb 2007 16:37:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  594
   SCR Title:  Remove issue with interpreting status response incorrectly
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   21 Sep 2005 07:38:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use new methods for accessing input list.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   21 Jun 2005 08:14:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   17 Feb 2005 14:41:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   16 Feb 2005 14:38:04   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   12 Feb 2004 16:52:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  374
   SCR Title:  Add check before changing tool parameters when processing input list
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Removed unused function that were originally designed for handling SetToolParameterList functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   22 Apr 2003 10:48:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   05 Mar 2003 12:13:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   30 Jan 2003 13:04:16   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated GetContextExtent, processMessage, onUpdateFigure, isRBResizeable, isBResizeable, isRResizeable, isMoveable and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   14 Jan 2003 14:03:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added methods (SetExtent)height, width, left top, SetAllExtents, GetExtent, SetToolPosition
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   10 Jan 2003 15:39:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   Added method to return the overlay figure from this object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   09 Dec 2002 19:32:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   19 Nov 2002 15:23:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed ASSERT's
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   08 Nov 2001 14:45:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated createObject method to calculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
