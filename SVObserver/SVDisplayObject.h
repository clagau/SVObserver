//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayObject
//* .File Name       : $Workfile:   SVDisplayObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:26  $
//******************************************************************************

#ifndef INC_SVDISPLAYOBJECT_INCLUDED
#define INC_SVDISPLAYOBJECT_INCLUDED

#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVInfoStructs.h"

class SVIPDoc;

class SVDisplayObject : 
	public SVObjectClass, 
	public SVObserverTemplate< SVInspectionCompleteInfoStruct >,
	public SVObserverTemplate< SVInspectionNameUpdate >,
	public SVObserverTemplate< SVRemoveImages >,
	public SVObserverTemplate< SVRemoveValues >,
	public SVObserverTemplate< SVRemoveSubjectStruct >
{
public:
	SVDisplayObject( LPCSTR ObjectName );
	SVDisplayObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVDISPLAYOBJECT );
	virtual ~SVDisplayObject();

	virtual HRESULT ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData );
	virtual HRESULT ObserverUpdate( const SVInspectionNameUpdate& p_rData );
	virtual HRESULT ObserverUpdate( const SVRemoveImages& p_rData );
	virtual HRESULT ObserverUpdate( const SVRemoveValues& p_rData );
	virtual HRESULT ObserverUpdate( const SVRemoveSubjectStruct& p_rData );

	void SetInspectionID( const SVGUID& p_rInspectionID );

	SVIPDoc* GetIPDoc() const;

	BOOL Create();
	BOOL Destroy();

	// Runtime Functions
	BOOL CanGoOnline();
	BOOL GoOnline();
	BOOL GoOffline();

	void SetIPDocDisplayComplete();

protected:
	static DWORD WINAPI SVDisplayThreadFunc( LPVOID lpParam );

	HRESULT ProcessInspectionComplete( bool& p_rProcessed );
	HRESULT ProcessNotifyIPDoc( bool& p_rProcessed );

	HRESULT FinishInspection( const SVInspectionCompleteInfoStruct& p_rProduct );

	void UpdateNextDisplayEvent();

	SVGUID m_InspectionID;

	// HANDLEs to the Display events that may be needed by the threads
	HANDLE m_hStartEvent;
	HANDLE m_hStopEvent;
	HANDLE m_hDisplayThread;

	bool m_CriticalSectionCreated;
	CRITICAL_SECTION m_CriticalSection;

	long m_PendingTrigger;
	long m_CurrentTrigger;
	long m_DisplayedTrigger;

	long m_IPDocDisplayComplete;

	long m_FrameRate;
	SVClock::SVTimeStamp m_NextDisplayEvent;
	SVClock::SVTimeStamp m_LastUpdateTime;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDisplayObject.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:02:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   29 Apr 2013 14:10:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:14:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Jul 2012 16:45:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   28 Jun 2012 17:52:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed display control functionally.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Jun 2012 12:40:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   14 May 2012 14:18:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic for freeze display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   03 Oct 2011 10:04:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the handshaking between the display object thread and the image view draw functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Sep 2011 15:53:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated display fucntionality based on new resource management methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Apr 2011 08:06:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Dec 2010 07:59:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Feb 2010 14:19:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use an event to cause the display to update instead of the APC functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Dec 2009 15:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Sep 2009 10:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Jul 2009 13:00:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Nov 2005 08:04:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_hResultImagePublishedCircleBuffer
 * changed DataManager SV_OTHER to SV_DISPLAY
 * changed long m_lResultImageIndex to SVImageIndexStruct m_svResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Oct 2005 08:59:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new control variable for the camera info struct map to determine if a new display operation is allowed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Aug 2005 10:11:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Feb 2005 13:30:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 May 2003 13:16:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Apr 2003 17:49:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed display logic to make a copy of the result indexes from the product before starting the SLOW display process so that the product can't be recycled before the display finishes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 09:02:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:54:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
