// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateLineStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateLineStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:01:42  $
// ******************************************************************************

#ifndef SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H
#define SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxEnums.h"

/**
@SVObjectName Matrox Buffer Create Line Structure

@SVObjectOverview This class is a data holder for a creating a 1d buffer or a line buffer.

@SVObjectOperations This data class has a set image depth function.

*/
class SVMatroxBufferCreateLineStruct  
{
public:
	SVMatroxBufferCreateLineStruct();
	virtual ~SVMatroxBufferCreateLineStruct();
	bool SetImageDepth( long p_lDepth);

	// See help on MbufAlloc1d.
	long					m_lSizeX;
	SVMatroxBufferTypeEnum	m_eType;
	SVMatroxBufferAttributeEnum	m_eAttribute;

};

#endif // #ifndef SV_MATROX_BUFFER_CREATE_LINE_STRUCT_H


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferCreateLineStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:01:42   bWalter
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
