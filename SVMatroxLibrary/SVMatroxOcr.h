// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcr
// * .File Name       : $Workfile:   SVMatroxOcr.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:12  $
// ******************************************************************************

#ifndef SV_MATROX_OCR_H
#define SV_MATROX_OCR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxTypedefs.h"

class SVMatroxOcrInterface;

/**
@SVObjectName Matrox Ocr

@SVObjectOverview This is a data class that holds a font object identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxOcr  
{
	friend SVMatroxOcrInterface;
public:
	SVMatroxOcr();
	virtual ~SVMatroxOcr();
	bool empty() const;
	bool operator<(const SVMatroxOcr& p_rBuf) const {return m_OcrFontID < p_rBuf.m_OcrFontID;};
	bool operator==( const SVMatroxOcr& p_rhs ) const ;

	bool				m_bVerify;
	SVMatroxString		m_VerifyString;

protected:
	SVMatroxIdentifier	m_OcrFontID;

private:
	void LocalClear();
};

#endif // #ifndef SV_MATROX_OCR_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxOcr.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:21:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:21:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
