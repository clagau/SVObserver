// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackStruct
// * .File Name       : $Workfile:   SVCallbackStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:44  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCallbackStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVCallbackStruct::SVCallbackStruct()
{
	m_pCallback = NULL;

	m_pOwner = NULL;
	m_pData = NULL;
}

SVCallbackStruct::SVCallbackStruct( const SVCallbackStruct &p_rOriginal )
{
	m_pCallback = NULL;

	m_pOwner = NULL;
	m_pData = NULL;

	*this = p_rOriginal;
}

SVCallbackStruct::~SVCallbackStruct()
{
}

const SVCallbackStruct &SVCallbackStruct::operator=( const SVCallbackStruct &p_rOriginal )
{
	m_pCallback = p_rOriginal.m_pCallback;

	m_pOwner = p_rOriginal.m_pOwner;
	m_pData = p_rOriginal.m_pData;

	return *this;
}

bool operator==( const SVCallbackStruct &p_rLeft,
                 const SVCallbackStruct &p_rRight )
{
	bool l_bOk = true;

	l_bOk = p_rLeft.m_pCallback == p_rRight.m_pCallback && l_bOk;
	l_bOk = p_rLeft.m_pOwner == p_rRight.m_pOwner && l_bOk;
	l_bOk = p_rLeft.m_pData == p_rRight.m_pData && l_bOk;

	return l_bOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVCallbackStruct.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:00:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:13:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New files needed to handle callback functionality and parameter functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/