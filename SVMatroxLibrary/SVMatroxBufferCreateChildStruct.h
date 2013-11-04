// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateChildStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateChildStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:16  $
// ******************************************************************************

#ifndef SV_MATROX_BUFFER_CREATE_CHILD_STRUCT_H
#define SV_MATROX_BUFFER_CREATE_CHILD_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxBuffer.h"


/**
@SVObjectName Matrox Buffer Create Child Struct

@SVObjectOverview This class is a data holder for a creating child buffers.

@SVObjectOperations This data class has no operations.

*/
class SVMatroxBufferCreateChildStruct  
{
public:
	SVMatroxBufferCreateChildStruct(const SVMatroxBuffer& p_rBuf);
	virtual ~SVMatroxBufferCreateChildStruct();

	// Specifies the identifier of the parent buffer.
	const SVMatroxBuffer& m_ParentBufId;

	// Specifies the color band of the parent data buffer from which to allocate 
	// the child data buffer. The specified color band should be valid in the 
	// parent buffer. 
	long m_lBand;

	// Specifies the number of bands of the child buffer to create.
	long m_lParentBandCount;

	// Specifies the horizontal offset of the child buffer, relative to the 
	// parent buffer's top-left pixel. The offset must be within the width of 
	// the parent buffer
	long m_lOffX;

	// Specifies the vertical offset of the child buffer, relative to the parent 
	// buffer's top-left pixel. The offset must be within the height of the 
	// parent buffer.
	long m_lOffY; 

	// Specifies the width of the child buffer. 
	long m_lSizeX; 

	// Specifies the height of the child buffer.
	long m_lSizeY; 

};

#endif // #ifndef SV_MATROX_BUFFER_CREATE_CHILD_STRUCT_H


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:00:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jul 2008 09:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  630
 * SCR Title:  Fix Matrox interface bug in color system
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ParentBandCount to createChildStruct to determine what kind of buffer to create.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
