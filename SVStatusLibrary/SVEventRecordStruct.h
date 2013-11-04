// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEventRecordClass
// * .File Name       : $Workfile:   SVEventRecordStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 17:43:00  $
// ******************************************************************************
#include <string>
#ifndef _SVEVENTRECORDCLASS_H
#define _SVEVENTRECORDCLASS_H

static const unsigned long SV_EVENT_RECORD_STRUCT_RECORD_SIZE = 0x0008000;

struct SVEventRecordStruct
{
	SVEventRecordStruct();
	virtual ~SVEventRecordStruct();

	void SetBuffer( unsigned char* p_pucBuffer, unsigned long p_ulSize );

	EVENTLOGRECORD* GetEventLogRecord();

	LPCTSTR GetSourceName();
	LPCTSTR GetComputerName();

	LPCTSTR GetFirstString();
	LPCTSTR GetNextString();
	LPCTSTR GetType();
	std::string GetDateTime();

private:
	HRESULT LocalInitialize(unsigned long p_ulSize);
	HRESULT LocalDestroy();

	unsigned char *m_pucBuffer;
	unsigned long m_ulBufferSize;

	EVENTLOGRECORD* m_poRecord;

	LPCTSTR m_szSourceName;
	LPCTSTR m_szComputerName;

  LPCTSTR m_szCurrentString;
	unsigned long m_ulStringIndex;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVStatusLibrary\SVEventRecordStruct.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:43:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Nov 2009 13:29:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed unreliable read from event log.  Added functions to aid in debugging.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Nov 2009 14:52:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed max size from 0x80000 to 0x8000.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jun 2008 13:38:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   fixed problem with reading from the event viewer
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
 *    Rev 1.3   26 Jul 2006 12:54:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed the buffer size to be an unsigned long
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jul 2006 14:40:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem if the EventView Record is longer then what is allocated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2006 11:49:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  561
 * SCR Title:  Change RAID Status dialog to only report RAID Error Status Messages
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Event Record Size to make sure the event record size and the event log buffer are the same size.
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
