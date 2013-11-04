// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalStruct
// * .File Name       : $Workfile:   SVIODigitalStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIODigitalStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVIODigitalStruct::SVIODigitalStruct()
{
	m_bIsInverted = false;
	m_bIsForced = false;
	m_bForcedValue = false;
	m_bValue = false;
}

SVIODigitalStruct::SVIODigitalStruct( const SVIODigitalStruct &p_rOriginal )
{
	m_bIsInverted = false;
	m_bIsForced = false;
	m_bForcedValue = false;
	m_bValue = false;

	*this = p_rOriginal;
}

SVIODigitalStruct::~SVIODigitalStruct()
{
}

const SVIODigitalStruct &SVIODigitalStruct::operator=( const SVIODigitalStruct &p_rOriginal )
{
	m_bIsInverted = p_rOriginal.m_bIsInverted;
	m_bIsForced = p_rOriginal.m_bIsForced;
	m_bForcedValue = p_rOriginal.m_bForcedValue;
	m_bValue = p_rOriginal.m_bValue;

	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIODigitalStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:46:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:21:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/