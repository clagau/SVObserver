// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrCreateStruct
// * .File Name       : $Workfile:   SVMatroxOcrCreateStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:34  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxOcrCreateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcrCreateStruct::SVMatroxOcrCreateStruct()
	:m_eFontType(SVGeneral),
	m_lCharNumber(0),
	m_lCharCellSizeX(18),
	m_lCharCellSizeY(22),
	m_lCharOffsetX(0),
	m_lCharOffsetY(0),
	m_lCharSizeX(12),
	m_lCharSizeY(16),
	m_lCharThickness(2),
	m_lStringLength(5),
	m_eInitFlag(SVOcrForegroundBlack)
{
}

SVMatroxOcrCreateStruct::~SVMatroxOcrCreateStruct()
{

}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxOcrCreateStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:24:34   bWalter
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
