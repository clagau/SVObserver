//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQShiftRegister
//* .File Name       : $Workfile:   SVPPQShiftRegister.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:16  $
//******************************************************************************

#ifndef SVPPQSHIFTREGISTER_H
#define SVPPQSHIFTREGISTER_H

#include <vector>
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"

struct SVProductInfoStruct;

class SVPPQShiftRegister
{
public:
	SVPPQShiftRegister();
	virtual ~SVPPQShiftRegister();

	size_t size() const;
	void resize( size_t p_Size );
	void clear();

	SVProductInfoStruct* GetProductAt( size_t p_Index ) const;

	HRESULT SetProductAt( size_t p_Index, SVProductInfoStruct* p_pProduct );

	SVProductInfoStruct* IndexRegister();

	HRESULT IsProductAlive( long p_TriggerCount ) const;

	HRESULT GetIndexByTriggerCount( long& p_rIndex, long p_TriggerCount ) const;
	HRESULT GetProductByTriggerCount( SVProductInfoStruct*& p_rpProduct, long p_TriggerCount ) const;

	HRESULT GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds = 0.0 ) const;
	HRESULT GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds, SVClock::SVTimeStamp p_UpperThresholdInMilliseconds ) const;

	HRESULT GetProductStates( SVString& p_rProductStates ) const;

private:
	typedef std::vector< SVProductInfoStruct* > SVProductVector;

	SVProductVector m_Products;

	long m_HeadTriggerCount;
	long m_TailTriggerCount;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPPQShiftRegister.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:31:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 14:25:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:33:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2012 13:49:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Sep 2011 16:18:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Feb 2010 09:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to move the indexing of the PPQ to the PPQ thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Feb 2010 10:43:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new files to separate PPQ functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
