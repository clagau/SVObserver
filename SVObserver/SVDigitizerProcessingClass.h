//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitizerProcessingClass
//* .File Name       : $Workfile:   SVDigitizerProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:46  $
//******************************************************************************

#ifndef SVDIGITIZERPROCESSINGCLASS_H
#define SVDIGITIZERPROCESSINGCLASS_H

#include <map>
#include <set>
#include <utility>
#include "SVUtilityLibrary/SVString.h"
#include "SVAcquisitionClass.h"

class SVDigitizerLoadLibraryClass;

class SVDigitizerProcessingClass
{
public:
	friend class SVObserverApp;

	static SVDigitizerProcessingClass& Instance();

	virtual ~SVDigitizerProcessingClass();

	void clear();

	void ClearDevices();

	HRESULT UpdateDigitizerSubsystem( SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem );

	HRESULT GetDigitizerList( CStringArray& rList ) const;
	HRESULT GetAcquisitionDeviceList( CStringArray& rList ) const;

	bool IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const;
	SVDigitizerLoadLibraryClass* GetDigitizerSubsystem( LPCTSTR digitizerName ) const;

	SVAcquisitionClassPtr GetDigitizer( LPCTSTR szName ) const;
	SVAcquisitionClassPtr GetAcquisitionDevice( LPCTSTR szName ) const;

	HRESULT DestroyBuffers();

	HRESULT ConnectDevices();
	HRESULT DisconnectDevices();

	HRESULT ScanForCameras();

	HRESULT StoreLastCameraImage();
	HRESULT RestoreLastCameraImage();

protected:
	SVDigitizerProcessingClass();

	void Startup();
	void Shutdown();

private:
	typedef std::set< SVDigitizerLoadLibraryClass* > SVDigitizerSubsystemSet;
	typedef std::map< SVString, SVAcquisitionClassPtr > SVNameDigitizerMap;
	typedef std::map< SVString, SVDigitizerLoadLibraryClass* > SVNameDigitizerSubsystemMap;

	HRESULT AddDigitizer( LPCTSTR p_szName, SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem, unsigned long p_Handle );

	HRESULT UpdateIntekDevices();
	HRESULT UpdateMatroxDevices();

	//Do not implement this method. This definition prevent copy and assignement operations.
	SVDigitizerProcessingClass( const SVDigitizerProcessingClass& p_rObject );

	//Do not implement this method. This definition prevent copy and assignement operations.
	const SVDigitizerProcessingClass& operator=( const SVDigitizerProcessingClass& p_rObject );

	SVDigitizerSubsystemSet m_Subsystems;

	SVNameDigitizerMap m_Digitizers;
	SVNameDigitizerMap m_AcquisitionDevices;

	SVNameDigitizerSubsystemMap m_DigitizerSubsystems;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDigitizerProcessingClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:11:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Mar 2011 16:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   22 Mar 2011 07:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   24 Feb 2010 10:30:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to handle storing and restoring the image buffers when the system is going on and off line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Feb 2010 14:17:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new construction functionality via the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Feb 2010 12:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Dec 2009 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jul 2009 11:24:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Oct 2008 17:54:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AddAvailableDigitizer method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2008 13:46:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetDigitizerSubsystem, GetImageSubsystem, IsValidDigitizerSubsystem, and IsValidImageSubsystem to use device name to support multiple acquisition devices.
 * 
 * Added map of Digitizer Devcie Name, Digitizer Device Interface and Image Interface pairs to support mulitple Digitizer Devices.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2005 15:47:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to include a method to give the digitizer count.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2005 13:30:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jul 2003 10:31:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made Subsystem member vars protected, added helper functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:02:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New class added to complete the new I/O interface methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
