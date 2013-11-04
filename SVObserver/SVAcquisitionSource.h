//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionSource
//* .File Name       : $Workfile:   SVAcquisitionSource.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:36  $
//******************************************************************************

#ifndef SVACQUISITIONSOURCE_H
#define SVACQUISITIONSOURCE_H

#include "SVContainerLibrary/SVVector.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVFileNameArrayClass.h"

enum { AcNone, AcCamera, AcFile };

class SVAcquisitionSourceClass
{
public:
	SVAcquisitionSourceClass();
	SVAcquisitionSourceClass( const SVAcquisitionSourceClass& svASC );
	SVAcquisitionSourceClass(   int iAcqType, 
	                            int iAcqTrigType, 
	                            const SVFileNameArrayClass& svFileNameArray, 
	                            const SVImageInfoClass& svImageInfo, 
	                            const SVLightReference& svLightRefArray );
	SVAcquisitionSourceClass( int iAcqType, 
	                          int iAcqTrigType, 
	                          LPCTSTR szFileNameList, 
	                          const SVImageInfoClass& svImageInfo, 
	                          const SVLightReference& svLightRefArray );

	~SVAcquisitionSourceClass();

	const SVAcquisitionSourceClass& operator=(const SVAcquisitionSourceClass& svASC);

	SVFileNameArrayClass GetFileNameArray();
	
	BOOL SetFileNameArray( LPCTSTR szFileNameList );
	BOOL SetFileNameArray( const SVFileNameArrayClass& svFileNameArray );

public:
	int AcquisitionType;
	int AcquisitionTriggerType;
	SVImageInfoClass AcquisitionImageInfo;
	SVLightReference AcquisitionLightReferenceArray;

private:
	SVFileNameArrayClass msvFileNameArray;

};

class SVAcquisitionSourceListClass : public SVVector< SVAcquisitionSourceClass* >
{
public:
	SVAcquisitionSourceListClass();
	SVAcquisitionSourceListClass(const SVAcquisitionSourceListClass& svASLC );

	~SVAcquisitionSourceListClass();

	const SVAcquisitionSourceListClass& operator=(const SVAcquisitionSourceListClass& svASLC);

	void DeleteAll();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAcquisitionSource.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:26:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Nov 2010 14:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   15 Dec 2009 14:55:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   21 Jun 2007 11:51:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   08 Jul 2003 08:43:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed from SVLightReferenceArray to SVLightReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Apr 2003 16:25:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   29 Jan 2003 10:22:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   18 Sep 2001 12:09:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  195
 * SCR Title:  Fix Random NAKs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed enumerated type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   12 Jun 2001 11:13:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVAcquisitionSourceClass, SVAcquisitionSourceClass, ~SVAcquisitionSourceClass, operator=, BOOL SVAcquisitionSourceClass::SetFileNameArray, SetFileNameArray, Serialize, SVAcquisitionSourceListClass, operator=.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   25 Jan 2001 16:14:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   22 Nov 1999 17:13:34   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Aug 11 1999 20:59:50   robert
 * Project:  SVO_200Custom
 * Change Request (SCR) nbr:  21
 * SCR Title:  Support as many PPQ's as digitizers are available on a System.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Split out from System Module to make files more managable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
