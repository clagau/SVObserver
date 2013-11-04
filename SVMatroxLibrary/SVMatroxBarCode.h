// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCode
// * .File Name       : $Workfile:   SVMatroxBarCode.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:36  $
// ******************************************************************************

#ifndef SV_MATROX_BAR_CODE_H
#define SV_MATROX_BAR_CODE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxTypedefs.h"

class SVMatroxBarCodeInterface;


/**
@SVObjectName Matrox Bar Code

@SVObjectOverview This is a data class that holds the identifier for a bar code.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/
class SVMatroxBarCode  
{
	friend SVMatroxBarCodeInterface;
public:
	SVMatroxBarCode();
	virtual ~SVMatroxBarCode();

	bool empty() const;
	SVMatroxBarCode& operator=( const SVMatroxBarCode& p_rhs );
	bool operator<(const SVMatroxBarCode& p_rBuf) const {return m_BarCodeId < p_rBuf.m_BarCodeId;};

protected:
	SVMatroxIdentifier m_BarCodeId;

private:
	void LocalClear();
};

#endif // !defined(SV_MATROX_BAR_CODE_H)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBarCode.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
