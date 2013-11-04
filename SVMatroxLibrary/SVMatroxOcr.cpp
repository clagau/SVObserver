// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcr
// * .File Name       : $Workfile:   SVMatroxOcr.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:22:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxOcr.h"
#include "SVMatroxOcrInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcr::SVMatroxOcr()
: m_OcrFontID(0), m_bVerify(false)
{
}

SVMatroxOcr::~SVMatroxOcr()
{
//	if( m_OcrFontID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxOcr::empty() const
{
	return m_OcrFontID == 0;
}

void SVMatroxOcr::LocalClear()
{
	SVMatroxOcrInterface l_Interface;
	l_Interface.Destroy( *this );
}

bool SVMatroxOcr::operator==( const SVMatroxOcr& p_rhs ) const
{
	return m_OcrFontID == p_rhs.m_OcrFontID;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxOcr.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:22:18   bWalter
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
