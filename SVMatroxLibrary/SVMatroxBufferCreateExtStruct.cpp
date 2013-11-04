// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateExtStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateExtStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:22  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBufferCreateExtStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateExtStruct::SVMatroxBufferCreateExtStruct()
: m_lSizeBand( 1 ), m_lSizeX(10), m_lSizeY(10),
 m_PitchWidth(0), m_ppArrayOfDataPtr(NULL)
{
	m_eType = SV8BitUnsigned;
	m_eAttribute = SVBufAttUnknown;
	m_eControlFlag = SVBufInfoUnknown;
}

SVMatroxBufferCreateExtStruct::~SVMatroxBufferCreateExtStruct()
{

}

bool SVMatroxBufferCreateExtStruct::SetImageDepth( long p_lDepth)
{
	bool l_bRet = true;
	if( p_lDepth == 1 )
	{
		m_eType = SV1BitUnsigned;
	}
	else if( p_lDepth <= 8 )
	{
		m_eType = SV8BitUnsigned;
	}
	else if( p_lDepth <= 16 )
	{
		m_eType = SV16BitUnsigned;
	}
	else if( p_lDepth <= 32 )
	{
		m_eType = SV32BitUnsigned;
	}
	else
	{
		l_bRet = false;
	}
	return l_bRet;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferCreateExtStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:00:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
