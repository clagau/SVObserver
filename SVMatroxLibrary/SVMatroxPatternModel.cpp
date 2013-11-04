// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatternModel
// * .File Name       : $Workfile:   SVMatroxPatternModel.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:26:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxPatternModel.h"
#include "SVMatroxPatternInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxPatternModel::SVMatroxPatternModel()
:m_ModelId(0)
{
}

SVMatroxPatternModel::~SVMatroxPatternModel()
{
//	if( m_ModelId != 0 )
//	{
//		LocalClear();
//	}

}

bool SVMatroxPatternModel::empty() const
{
	return m_ModelId == 0;
}

void SVMatroxPatternModel::LocalClear()
{
	SVMatroxPatternInterface l_Interface;
	l_Interface.Destroy( *this );
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxPatternModel.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:26:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:22:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
