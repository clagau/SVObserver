//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVMainImageClass
//* .File Name       : $Workfile:   SVMainImageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVMainImageClass.h"

#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"

#include "SVAcquisitionClass.h"
#include "SVConfigurationObject.h"
#include "SVObserver.h"

// Wasn't in old implementation, take back out for now. may be needed in future.
// SV_IMPLEMENT_CLASS( SVMainImageClass, SVMainImageClassGuid );

SVMainImageClass::SVMainImageClass( LPCSTR ObjectName )
: SVCameraImageTemplate( ObjectName )
{
	init();
}

SVMainImageClass::SVMainImageClass( SVObjectClass* POwner, int StringResourceID )
: SVCameraImageTemplate( POwner, StringResourceID )
{
	init();
}

void SVMainImageClass::init()
{
	outObjectInfo.ObjectTypeInfo.SubType = SVMainImageObjectType;

	m_ImageType = SVImageTypeMain;
}

SVMainImageClass::~SVMainImageClass()
{
}

BOOL SVMainImageClass::SetImageHandleIndex( SVImageIndexStruct svIndex )
{
	BOOL bOk = TRUE;

	m_CurrentIndex = svIndex;

	SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

	while( l_Iter != m_ChildArrays.end() )
	{
		if( l_Iter->second.m_pImageHandles != NULL )
		{
			bOk &= l_Iter->second.m_pImageHandles->SetCurrentIndex( svIndex.m_CameraDMIndexHandle );
		}

		++l_Iter;
	}
	
	return bOk;
}

BOOL SVMainImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	BOOL bOk = FALSE;
	
	return bOk;
}

BOOL SVMainImageClass::GetImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( m_CurrentIndex.m_CameraDMIndexHandle.GetIndex(), p_rHandlePtr );
	}
	
	return bOk;
}

BOOL SVMainImageClass::GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle )
{
	BOOL bOk = GetCameraBufferArrayPtr() != NULL;
	
	if ( bOk )
	{
		bOk = GetCameraBufferArrayPtr()->GetImageHandle( svIndex.m_CameraDMIndexHandle.GetIndex(), rHandle );
	}
	
	return bOk;
}

BOOL SVMainImageClass::OnValidate()
{
	if( SVObjectAppClass::OnValidate() )
	{
		isObjectValid = !( GetCameraBufferArrayPtr().empty() );
	}

	return isObjectValid;
}

const SVClock::SVTimeStamp& SVMainImageClass::GetLastResetTimeStamp() const
{
	if( GetCameraBufferArrayPtr() != NULL )
	{
		m_LastReset = std::max( m_LastReset, GetCameraBufferArrayPtr()->GetLastResetTimeStamp() );
	}

	return SVCameraImageTemplate::GetLastResetTimeStamp();
}
	
BOOL SVMainImageClass::CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr )
{
	BOOL bOk = !( p_ImageArrayPtr.empty() );

	if ( bOk )
	{
		// Get Basic image info from ACB...
		m_ImageInfo = p_rImageInfo;

		// Set up necessary main image info...
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();
	}

	return bOk;      
}

SVImageObjectClassPtr SVMainImageClass::GetBufferArrayPtr() const
{
	return GetCameraBufferArrayPtr();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMainImageClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:16:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   29 Apr 2013 14:18:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:26:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   28 Sep 2011 13:09:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   16 Sep 2011 16:05:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   13 Apr 2011 15:29:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   30 Mar 2011 16:24:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 Mar 2011 13:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   25 Feb 2011 12:17:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   10 Feb 2011 15:00:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   01 Feb 2011 11:56:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   27 Jan 2011 11:33:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   08 Dec 2010 13:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   09 Nov 2010 16:21:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   04 Nov 2010 13:43:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   28 Oct 2010 14:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   24 Feb 2010 10:42:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new store and restore methodolgy for camera image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Dec 2009 12:00:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   30 Jul 2009 12:04:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   06 Mar 2008 16:05:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * and
 * Added Attributes to support Image Definition Lists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22.1.0   14 Jul 2009 14:37:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated with new image methods and data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   17 Jan 2006 09:58:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History attributes
 * SVImageBufferClass was renamed to SVImageOverlayClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Nov 2005 12:42:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CreateObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Nov 2005 09:09:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   08 Sep 2005 12:30:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added methods to store the camera image to a BSTR and restore the images back via the SVInspectionProcess.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Aug 2005 10:19:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2005 08:15:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SetObjectValue now returns SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Mar 2005 11:00:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added comment for future change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Feb 2005 14:48:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Nov 2003 12:23:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Sep 2003 09:23:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made DisconnectBuffers recursive
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 May 2003 14:11:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed MainImage and RGBMainImage to work for the color system as well as the RGB mono systems by correctly calling the correct CreateBufferArray function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 May 2003 11:35:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed call to CreateBufferArrays from init cause it broke the color tool. We need to find a different solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Apr 2003 10:33:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In, ::init() added call to CreateBufferArrays()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 11:07:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Mar 2003 15:26:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added code to rebuild the PPQs input list after the PPQ is rebuilt with the new camera from an acquistion tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Mar 2003 16:13:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  296
 * SCR Title:  Fix PPQ when multiple inspections and cameras are used
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   mad code change so that the PPQ gets rebuilt after an acquisiton tool's camera is added to that PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Mar 2003 17:38:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SetImageHandleIndex to fix a problem with RGBMainImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Mar 2003 11:54:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed the ClassRegister macros that were previously added. They may be needed in the future.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Mar 2003 16:17:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  337
 * SCR Title:  Memory leak when editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods for DisconnectBuffers and ReconnectBuffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Feb 2003 14:57:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated Destroy to destroy the mpBufferArray
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Feb 2003 16:40:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added ReserverImageHandleIndex and ReleaseImageHandleIndex methods to handle run once and regression operations.  Modified CreateBuffers and SetImageDepth methods to initialize main image to -1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:06:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved source code into new files and updated all methods to use new buffer handling methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
