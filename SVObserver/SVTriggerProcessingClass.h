//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerProcessingClass
//* .File Name       : $Workfile:   SVTriggerProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2013 18:06:38  $
//******************************************************************************

#ifndef SVTRIGGERPROCESSINGCLASS_H
#define SVTRIGGERPROCESSINGCLASS_H

#include <map>
#include <set>
#include "SVUtilityLibrary/SVString.h"

class SVTriggerClass;
class SVIOTriggerLoadLibraryClass;

class SVTriggerProcessingClass
{
public:
	friend class SVObserverApp;

	static SVTriggerProcessingClass& Instance();

	virtual ~SVTriggerProcessingClass();

	void clear();

	HRESULT UpdateTriggerSubsystem( SVIOTriggerLoadLibraryClass* p_pDLLTrigger );

	SVTriggerClass* GetTrigger( LPCTSTR p_szName ) const;

protected:
	SVTriggerProcessingClass();

	void Startup();
	void Shutdown();

private:
	typedef std::set< SVIOTriggerLoadLibraryClass* > SVTriggerSubsystemSet;
	typedef std::map< SVString, SVTriggerClass* > SVNameTriggerMap;
	typedef std::map< SVString, SVIOTriggerLoadLibraryClass* > SVNameTriggerSubsystemMap;

	HRESULT AddTrigger( LPCTSTR p_szName, SVIOTriggerLoadLibraryClass* p_pTriggerSubsystem, unsigned long p_Handle );

	//Do not implement this method. This definition prevent copy and assignement operations.
	SVTriggerProcessingClass( const SVTriggerProcessingClass* p_rObject );

	//Do not implement this method. This definition prevent copy and assignement operations.
	const SVTriggerProcessingClass& operator=( const SVTriggerProcessingClass* p_rObject );

	SVTriggerSubsystemSet m_Subsystems;

	// these are specific to each board
	SVNameTriggerMap m_Triggers;

	SVNameTriggerSubsystemMap m_TriggerSubsystems;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerProcessingClass.h_v  $
 * 
 *    Rev 1.1   23 Jul 2013 18:06:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std map
 * Added include for std set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:51:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Jan 2011 12:02:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Dec 2010 13:48:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Feb 2010 14:40:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated device construction fucntionality to use new hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Feb 2010 12:42:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2009 13:23:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 17:03:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support more than one trigger DLL being loaded at a time.
 * Revised to support Software Trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Feb 2005 15:23:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2004 07:49:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the Create functions to allow the caller to supply the base name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Sep 2003 08:48:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed the type of the variable that interfaces with the hardware from SVIOConfigurationInterfaceClass to SVIOTriggerLoadLibraryClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 08:40:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added a new class to interface the Trigger sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
