//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventLogClass
//* .File Name       : $Workfile:   SVEventLogClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:42:30  $
//******************************************************************************

#ifndef _SVEVENTLOGCLASS_H
#define _SVEVENTLOGCLASS_H

#include "SVStatusLibrary/SVEventRecordStruct.h"
#include "SVUtilityLibrary/SVString.h"

class SVEventLogClass  
{
public:
	SVEventLogClass();
	virtual ~SVEventLogClass();

	HRESULT Open( LPCTSTR p_pszName );
	HRESULT Close();

	HRESULT GetRecordCount( unsigned long& p_rulCount );

	HRESULT ReadLast( SVEventRecordStruct& p_rsvRecord );
	HRESULT ReadPrevious( SVEventRecordStruct& p_rsvRecord );

private:

	HRESULT LocalInitialize();
	HRESULT LocalDestroy();

	SVString m_csName; // Should be one of the following: "Application", "Security", or "System"

	HANDLE m_hHandle;

	unsigned char *m_pucBuffer;
	unsigned long m_ulBufferSize;

	unsigned long m_ulBytesRead;

	unsigned char* m_pucPrevious;
	unsigned char* m_pucCurrent;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\SVEventLogClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:42:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jun 2007 15:12:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jul 2006 14:40:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem if the EventView Record is longer then what is allocated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2006 11:49:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Event Record Size to make sure the event record size and the event log buffer are the same size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Apr 2006 15:11:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  560
 * SCR Title:  Remove un-necessary event log read errors for SATA RAID status metholodolgy
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated read methods to ignore end-of-file contition and updated the size of the read buffer to 512K.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Dec 2005 15:08:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new objects to the library to read the Windows Event Log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
