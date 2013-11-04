//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeShim
//* .File Name       : $Workfile:   SVObjectAttributeShim.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectAttributeShim.h"
#include "SVObjectClass.h"

SVObjectAttributeShim::~SVObjectAttributeShim()
{
	UINT uChangedBits = m_uOriginal ^ m_rAttribute;	// find which bits are different
	if ( m_bUpdate )
		m_pOwner->SetObjectAttributesSet(m_rAttribute, m_lIndex, ~0);	// set all the bits
		//m_pOwner->SetObjectAttributesSet(m_rAttribute, m_lIndex, uChangedBits);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectAttributeShim.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:37:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2005 13:34:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/