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

