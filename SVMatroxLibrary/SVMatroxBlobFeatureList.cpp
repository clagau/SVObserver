// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobFeatureList
// * .File Name       : $Workfile:   SVMatroxBlobFeatureList.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBlobFeatureList.h"
#include "SVMatroxBlobInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBlobFeatureList::SVMatroxBlobFeatureList()
{
	m_BlobFeatureListID = 0;
}

SVMatroxBlobFeatureList::~SVMatroxBlobFeatureList()
{
//	if( m_BlobFeatureListID != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxBlobFeatureList::empty() const
{
	return m_BlobFeatureListID == 0;
}

void SVMatroxBlobFeatureList::LocalClear()
{
	SVMatroxBlobInterface l_Interface;
	l_Interface.Destroy( *this );
}


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBlobFeatureList.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
