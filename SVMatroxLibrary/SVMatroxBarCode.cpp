// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCode
// * .File Name       : $Workfile:   SVMatroxBarCode.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBarCode.h"
#include "SVMatroxBarCodeInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBarCode::SVMatroxBarCode()
:m_BarCodeId(0)
{
}

SVMatroxBarCode::~SVMatroxBarCode()
{
//	if( m_BarCodeId != 0 )
//	{
//		LocalClear();
//	}
}

bool SVMatroxBarCode::empty() const
{
	return m_BarCodeId == 0;
}

void SVMatroxBarCode::LocalClear()
{
	SVMatroxBarCodeInterface l_Interface;
	l_Interface.Destroy( *this );
}


SVMatroxBarCode& SVMatroxBarCode::operator=( const SVMatroxBarCode& p_rhs )
{
//	SVMatroxBarCodeInterface l_BarCodeIntf;
//	l_BarCodeIntf.Create( *this, p_rhs );
	m_BarCodeId = p_rhs.m_BarCodeId;
	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBarCode.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:30   bWalter
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
