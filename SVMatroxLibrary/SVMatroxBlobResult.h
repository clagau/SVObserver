// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobResult
// * .File Name       : $Workfile:   SVMatroxBlobResult.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:59:42  $
// ******************************************************************************

#ifndef SV_MATROX_BLOB_RESULT_H
#define SV_MATROX_BLOB_RESULT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxTypedefs.h"

class SVMatroxBlobInterface;

/**
@SVObjectName Matrox Blob Result

@SVObjectOverview This is a data class that holds a blob result identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxBlobResult  
{
	friend SVMatroxBlobInterface;
public:
	SVMatroxBlobResult();
	virtual ~SVMatroxBlobResult();

	bool empty() const;
	bool operator<(const SVMatroxBlobResult& p_rBuf) const {return m_BlobResultId < p_rBuf.m_BlobResultId;};

protected:
	SVMatroxIdentifier m_BlobResultId;

private:
	void LocalClear();
};

#endif // #ifndef SV_MATROX_BLOB_RESULT_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBlobResult.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:59:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
