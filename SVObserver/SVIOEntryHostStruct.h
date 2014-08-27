//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vission, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryHostStruct
//* .File Name       : $Workfile:   SVIOEntryHostStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   27 Aug 2014 01:24:38  $
//******************************************************************************

#ifndef SVIOENTRYHOSTSTRUCT_H
#define SVIOENTRYHOSTSTRUCT_H

#include <deque>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVObjectClass;
class SVValueObjectClass;

enum SVIOObjectType
{
	IO_INVALID_OBJECT	= 0x0000,	// 0000 0000 0000 0000
	IO_DIGITAL_INPUT	= 0x0001,	// 0000 0000 0000 0001
	IO_REMOTE_INPUT		= 0x0004,	// 0000 0000 0000 0100
	IO_DIGITAL_OUTPUT	= 0x0008,	// 0000 0000 0000 1000
	IO_REMOTE_OUTPUT	= 0x0020,	// 0000 0000 0010 0000
	IO_CAMERA_DATA_INPUT = 0x8000,	// 1000 0000 0000 0000
};

struct SVIOEntryHostStruct 
{
	SVIOEntryHostStruct();
	virtual ~SVIOEntryHostStruct();

	void clear();

	bool m_Enabled;
	long m_PPQIndex;

	SVIOObjectType m_ObjectType;

	SVGUID m_IOId;

	bool m_DeleteValueObject;
	SVValueObjectClass* m_pValueObject;
	SVObjectClass* m_pValueParent;

	static bool PtrGreater( SVSharedPtr< SVIOEntryHostStruct > elem1, SVSharedPtr< SVIOEntryHostStruct > elem2 );

private:
	SVIOEntryHostStruct( const SVIOEntryHostStruct& p_rsvObject );

	const SVIOEntryHostStruct& operator=( const SVIOEntryHostStruct& p_rsvObject );
};

typedef SVSharedPtr< SVIOEntryHostStruct > SVIOEntryHostStructPtr;
typedef std::deque< SVIOEntryHostStructPtr > SVIOEntryHostStructPtrList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIOEntryHostStruct.h_v  $
 * 
 *    Rev 1.3   27 Aug 2014 01:24:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Added the sort method PtrGreater
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2013 13:20:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Apr 2013 14:24:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:09:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jan 2013 10:56:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IO_CAMERA_DATA_INPUT to SVIOObjectType enum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2011 10:24:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
