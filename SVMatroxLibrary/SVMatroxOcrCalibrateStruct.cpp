// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrCalibrateStruct
// * .File Name       : $Workfile:   SVMatroxOcrCalibrateStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:22  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxOcrCalibrateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcrCalibrateStruct::SVMatroxOcrCalibrateStruct()
{
	m_TargetCharSizeXMin = 0;
	m_TargetCharSizeXMax = 0;
	m_TargetCharSizeXStep = 0;
	m_TargetCharSizeYMin = 0;
	m_TargetCharSizeYMax = 0; 
	m_TargetCharSizeYStep = 0;

}

SVMatroxOcrCalibrateStruct::~SVMatroxOcrCalibrateStruct()
{

}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxOcrCalibrateStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:24:22   bWalter
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
