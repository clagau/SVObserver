// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferCreateStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:01:52  $
// ******************************************************************************

#ifndef SV_MATROX_BUFFER_CREATE_STRUCT_H
#define SV_MATROX_BUFFER_CREATE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxEnums.h"

/**
@SVObjectName Matrox Buffer Create Structure

@SVObjectOverview This class is a data holder for a creating a buffer.

@SVObjectOperations This data class has a set image depth function.

*/
class SVMatroxBufferCreateStruct  
{
public:
	SVMatroxBufferCreateStruct();
	virtual ~SVMatroxBufferCreateStruct();

	// See help on MbufAllocColor.
	long						m_lSizeBand;
	long						m_lSizeX;
	long						m_lSizeY;
	SVMatroxBufferTypeEnum		m_eType;
	SVMatroxBufferAttributeEnum	m_eAttribute;
	bool SetImageDepth( long p_lDepth);
};

#endif // #ifndef SV_MATROX_BUFFER_CREATE_STRUCT_H


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferCreateStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:01:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
