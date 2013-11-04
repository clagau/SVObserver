//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeShim
//* .File Name       : $Workfile:   SVObjectAttributeShim.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:46:20  $
//******************************************************************************

#ifndef SVOBJECTATTRIBUTESHIM_H_
#define SVOBJECTATTRIBUTESHIM_H_
#pragma once

// This class is intended to allow the SVObjectClass::ObjectAttributesSetRef
// to work seamlessly with arrays and non-arrays.
// The class acts like a UINT&, but when it's destructor is run,
// it calls the SVObjectClass::SetObjectAttributesSet(UINT) member function
// with its updated value.

class SVObjectClass;

const UINT SET_ALL_BITS = ~0;

class SVObjectAttributeShim
{
public:
	SVObjectAttributeShim( SVObjectClass* pOwner, UINT& rAttribute, long lIndex );
	SVObjectAttributeShim( SVObjectAttributeShim& rhs );
	~SVObjectAttributeShim();
	operator volatile UINT& ();
	const SVObjectAttributeShim& operator =  ( UINT uAttribute );
	const SVObjectAttributeShim& operator &= ( UINT uAttribute );
	const SVObjectAttributeShim& operator |= ( UINT uAttribute );
private:
	volatile UINT& m_rAttribute;
	SVObjectClass* m_pOwner;
	long m_lIndex;
	bool m_bUpdate;
	UINT m_uOriginal;
};

inline SVObjectAttributeShim::SVObjectAttributeShim( SVObjectClass* pOwner, UINT& rAttribute, long lIndex )
	: m_pOwner ( pOwner ), m_rAttribute ( rAttribute ), m_uOriginal ( rAttribute ), m_lIndex ( lIndex ), m_bUpdate ( true )
{
}

inline SVObjectAttributeShim::SVObjectAttributeShim( SVObjectAttributeShim& rhs )
	: m_pOwner ( rhs.m_pOwner ), m_rAttribute ( rhs.m_rAttribute ), m_uOriginal ( rhs.m_uOriginal ), m_lIndex ( rhs.m_lIndex ), m_bUpdate ( true )
{
	rhs.m_bUpdate = false;
}

inline SVObjectAttributeShim::operator volatile UINT& ()
{
	return m_rAttribute;
}

inline const SVObjectAttributeShim& SVObjectAttributeShim::operator = ( UINT uAttribute )
{
	m_rAttribute = uAttribute;
	return *this;
}

inline const SVObjectAttributeShim& SVObjectAttributeShim::operator &= ( UINT uAttribute )
{
	m_rAttribute &= uAttribute;
	return *this;
}

inline const SVObjectAttributeShim& SVObjectAttributeShim::operator |= ( UINT uAttribute )
{
	m_rAttribute |= uAttribute;
	return *this;
}


#endif	// #ifndef SVOBJECTATTRIBUTESHIM_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectAttributeShim.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:46:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2005 13:34:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/