//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRGBMainImage
//* .File Name       : $Workfile:   SVRGBMainImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:43:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVRGBMainImage.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVImageObjectClass.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"

SVRGBMainImageClass::SVRGBMainImageClass( LPCSTR ObjectName )
: SVCameraImageTemplate( ObjectName )
{
	outObjectInfo.ObjectTypeInfo.SubType = SVRGBMainImageObjectType;

	m_ImageType = SVImageTypeRGBMain;

	// Set up default for Band Number...
	m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );	// RGB Image
}

SVRGBMainImageClass::SVRGBMainImageClass( SVObjectClass* POwner, int StringResourceID )
: SVCameraImageTemplate( POwner, StringResourceID )
{
	outObjectInfo.ObjectTypeInfo.SubType = SVRGBMainImageObjectType;

	m_ImageType = SVImageTypeRGBMain;

	// Set up default for Band Number...
	m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );	// RGB Image
}

SVRGBMainImageClass::~SVRGBMainImageClass()
{
}

HRESULT SVRGBMainImageClass::ResetObject()
{
	HRESULT l_hrOk = SVCameraImageTemplate::ResetObject();

	if( ! UpdateBuffer() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVRGBMainImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	return SetImageHandleIndex( svIndex );
}

BOOL SVRGBMainImageClass::SetImageHandleIndex(SVImageIndexStruct svIndex)
{
	BOOL bOk = SVCameraImageTemplate::SetImageHandleIndex( svIndex );

	m_CurrentIndex = svIndex;

	bOk &= UpdateBuffer();

	return bOk;
}

BOOL SVRGBMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr )
{
	BOOL bOk = !( p_ImageArrayPtr.empty() );

	long l_lDepth = 0;

	bOk = bOk && SetImageDepth( GetImageDepth() );

	if ( bOk )
	{
		long l_lBandNumber = 1;

		m_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );

		// Get Basic image info from ACB...
		m_ImageInfo = p_rImageInfo;

		// Set up necessary main image info...
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, l_lBandNumber );

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	
	return bOk;      
}

BOOL SVRGBMainImageClass::GetCameraImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( m_CurrentIndex.m_CameraDMIndexHandle.GetIndex(), p_rHandlePtr );
	}
	
	return bOk;
}

BOOL SVRGBMainImageClass::GetCameraImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( svIndex.m_CameraDMIndexHandle.GetIndex(), rHandle );
	}
	
	return bOk;
}

BOOL SVRGBMainImageClass::UpdateBuffer()
{
	BOOL bOk = TRUE;

	// Check for Dependents
	SVSmartHandlePointer MainHandle;
	SVSmartHandlePointer CameraHandle;

	if( GetImageHandle( MainHandle ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		MainHandle->GetData( l_MilHandle );

		if( GetCameraImageHandle( CameraHandle ) )
		{
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVImageBufferHandleImage l_CameraMilHandle;
			CameraHandle->GetData( l_CameraMilHandle );

			// Converts it to Hue, Luminance, Saturation (HLS).
			l_Code = SVMatroxImageInterface::Convert( l_MilHandle.GetBuffer(), l_CameraMilHandle.GetBuffer(), SVImageRGBToHLS );
		}
		else
		{
			SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), 0 );
		}
	}
	else
	{
		bOk = FALSE;
	}

	return bOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRGBMainImage.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:43:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   29 Apr 2013 14:28:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:00:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   28 Sep 2011 13:13:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   16 Sep 2011 16:19:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   13 Apr 2011 15:31:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   30 Mar 2011 16:24:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   16 Mar 2011 13:51:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   25 Feb 2011 12:22:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   10 Feb 2011 15:11:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   01 Feb 2011 11:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   27 Jan 2011 11:54:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Dec 2010 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   04 Nov 2010 14:11:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Dec 2009 12:34:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 12:24:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   25 Jul 2007 07:48:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16.1.0   14 Jul 2009 15:10:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   02 Nov 2005 09:09:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Oct 2005 12:21:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the SetImageHandleIndex method to only update when the new index is different than the old index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   11 Mar 2005 13:32:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added member m_bResetting, Set in resetObject to allow SetImageHandleIndex to pass when resetting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Feb 2005 15:26:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Feb 2005 15:12:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Nov 2003 12:24:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 May 2003 14:11:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed MainImage and RGBMainImage to work for the color system as well as the RGB mono systems by correctly calling the correct CreateBufferArray function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Apr 2003 15:11:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Mar 2003 16:00:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated class to fix HSI problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Mar 2003 10:31:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added clear method to the class to make sure all images get cleared.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Mar 2003 17:40:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated CreateBuffers and SetImageHandleIndex methods to fix a problem with RGBMainImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jan 2003 16:19:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 13:12:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to accommodate new architecture
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Sep 2001 11:48:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  195
 * SCR Title:  Fix Random NAKs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method ReleaseCurrentBuffer to class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 18:57:56   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 15:18:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Sep 2000 09:27:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  184
 * SCR Title:  Color SVIM Display Image Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This fixed the wrong board problem in the destroy method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:40:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
