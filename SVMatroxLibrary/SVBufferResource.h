// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferResource
// * .File Name       : $Workfile:   SVBufferResource.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:14  $
// ******************************************************************************

#ifndef SVBUFFERRESOURCE_H
#define SVBUFFERRESOURCE_H

#include "mil.h"

// This class is used to provide a MIL_TEXT_CHAR buffer for use with mil functions.
class SVBufferResource
{
public:
	SVBufferResource(void);
	virtual ~SVBufferResource(void);
	void Destroy(void);
	HRESULT Resize( long l_lSize);
	HRESULT GetBuffer( MIL_TEXT_CHAR*& p_rpMilChar );
	MIL_TEXT_CHAR* m_pBuffer;
};

#endif // SVBUFFERRESOURCE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVBufferResource.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 May 2008 09:21:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix error in PVCS Log  comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
