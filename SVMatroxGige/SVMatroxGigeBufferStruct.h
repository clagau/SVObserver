//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferStruct
//* .File Name       : $Workfile:   SVMatroxGigeBufferStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:22  $
//******************************************************************************

#ifndef SVMATROXGIGEBUFFERSTRUCT_H
#define SVMATROXGIGEBUFFERSTRUCT_H

#include <deque>
//#include <vector>
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

typedef SVSharedPtr<SVMatroxBuffer> SVMatroxBufferRef;

class SVMatroxGigeBufferStruct
{
public:
	SVMatroxGigeBufferStruct();
	SVMatroxGigeBufferStruct( const SVMatroxGigeBufferStruct& p_rValue );
	virtual ~SVMatroxGigeBufferStruct();

	const SVMatroxGigeBufferStruct& operator=( const SVMatroxGigeBufferStruct& p_rValue );

	bool IsValid() const;

	unsigned long m_ulWidth;
	unsigned long m_ulHeight;
	unsigned long m_ulFormat;
	unsigned long m_ulAddress;
	bool m_bIsLocked;

	SVMatroxBufferRef m_Buffer;
};

typedef std::deque<SVMatroxGigeBufferStruct> SVMatroxGigeBufferStructArray;
//typedef std::vector<SVMatroxGigeBufferStruct> SVMatroxGigeBufferStructArray;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeBufferStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:41:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
