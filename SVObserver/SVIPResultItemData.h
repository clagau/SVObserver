//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemData
//* .File Name       : $Workfile:   SVIPResultItemData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:16:16  $
//******************************************************************************

#ifndef SVIPRESULTITEMDATA_H
#define SVIPRESULTITEMDATA_H

#include "SVUtilityLibrary/SVString.h"

class SVIPResultItemData
{
public:
	SVIPResultItemData();
	SVIPResultItemData( const SVString& p_rValue, unsigned long p_Color );
	SVIPResultItemData( const SVString& p_rValue, unsigned long p_Color, unsigned long p_IOType );
	SVIPResultItemData( const SVIPResultItemData& p_rObject );

	const SVString& GetValue() const;
	unsigned long GetColor() const;
	bool IsIOTypePresent() const;
	unsigned long GetIOType() const;

protected:
	SVString m_Value;
	unsigned long m_Color;

	bool m_IOTypePresent;
	long m_IOType;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultItemData.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:16:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/