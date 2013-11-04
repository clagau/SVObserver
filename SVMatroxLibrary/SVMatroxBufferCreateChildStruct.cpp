// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateChildStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateChildStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:10  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBufferCreateChildStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateChildStruct::SVMatroxBufferCreateChildStruct(const SVMatroxBuffer& p_rBuf)
: m_lBand(0), m_lParentBandCount(1),	m_lOffX(0), m_lOffY(0), m_lSizeX(10), m_lSizeY(10), m_ParentBufId( p_rBuf )
{

}


SVMatroxBufferCreateChildStruct::~SVMatroxBufferCreateChildStruct()
{

}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferCreateChildStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:00:10   bWalter
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
