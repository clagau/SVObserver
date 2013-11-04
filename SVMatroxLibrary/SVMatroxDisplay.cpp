// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplay
// * .File Name       : $Workfile:   SVMatroxDisplay.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:26  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxDisplay.h"
#include "SVMatroxDisplayInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxDisplay::SVMatroxDisplay()
: m_DisplayIdentifier(0)
{
}

SVMatroxDisplay::~SVMatroxDisplay()
{
//	if( m_DisplayIdentifier != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxDisplay::empty() const
{
	return m_DisplayIdentifier == 0;
}

void SVMatroxDisplay::LocalClear()
{
	SVMatroxDisplayInterface l_Interface;
	l_Interface.Destroy( *this );
}


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplay.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
