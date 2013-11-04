// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferResource
// * .File Name       : $Workfile:   SVBufferResource.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:08  $
// ******************************************************************************
#include "stdafx.h"
#include "SVBufferResource.h"

SVBufferResource::SVBufferResource(void)
{
	m_pBuffer = NULL;
}

SVBufferResource::~SVBufferResource(void)
{
	Destroy();
}


void SVBufferResource::Destroy(void)
{
	if( m_pBuffer )
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}
HRESULT SVBufferResource::Resize( long l_lSize)
{
	Destroy();
	m_pBuffer = new char[l_lSize];
	if( m_pBuffer )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT SVBufferResource::GetBuffer( MIL_TEXT_CHAR*& p_rpMilChar )
{
	if( m_pBuffer )
	{
		p_rpMilChar = m_pBuffer;
		return S_OK;
	}
	else
	{
		p_rpMilChar = NULL;
		return S_FALSE;
	}
}


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVBufferResource.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 May 2008 09:21:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix error in PVCS Log  comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
