// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVReferenceCountingTemplate
// * .File Name       : $Workfile:   SVReferenceCountingTemplate.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:46:28  $
// ******************************************************************************

#ifndef SVREFERENCECOUNTINGTEMPLATE_H_
#define SVREFERENCECOUNTINGTEMPLATE_H_
#pragma once

// policy interface for RESOURCEMANAGER
template <typename RESOURCE>
struct DefaultResourceManager
{
	void DefaultCreate(RESOURCE*) {};
	void Create(RESOURCE*) {};	// not used by SVReferenceCountingTemplate
	void Destroy(RESOURCE*) {};
	void Clear(RESOURCE&) {};
};

// policy interface for ALLOCATIONPOLICY
struct DefaultAllocationPolicy
{
	void* Alloc(size_t size) { return malloc(size); }
	void  Free(void* p)      { free(p); }
};

template < typename RESOURCE, typename RESOURCEMANAGER = DefaultResourceManager<RESOURCE>, typename ALLOCATIONPOLICY = DefaultAllocationPolicy >
class SVReferenceCountingTemplate : public RESOURCEMANAGER, public ALLOCATIONPOLICY
{
public:
	typedef SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY> this_type;

	SVReferenceCountingTemplate();
	SVReferenceCountingTemplate( const SVReferenceCountingTemplate& rhs );
	SVReferenceCountingTemplate( RESOURCE& rhs );	// like std::auto_ptr, this takes control of a non-managed resource, hence the param is non-const. Clear is called on rhs.
	~SVReferenceCountingTemplate();
	const this_type& operator = ( const this_type& rhs );

	RESOURCE* Get();
	RESOURCE& GetRef();

	RESOURCE* operator -> ();
	const RESOURCE* operator -> () const;

	void swap( SVReferenceCountingTemplate& rhs );

protected:

	void Acquire( const SVReferenceCountingTemplate& rhs );
	void Release();

	RESOURCE* m_pResource;
	mutable volatile long* m_plRefCount;	// mutable to allow this class to be const-friendly

};

// we don't want to use the default std::swap because it will invoke copy constructors and destructors.
// Make the efficient member function available to algorithms:

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline void swap ( SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>& lhs, SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>& rhs )
{
	lhs.swap(rhs);
}

namespace std	// yes this is legal and good practice; see Effective C++ 3rd ed. Item 25
{
	template< typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
	inline void swap( SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>& lhs, SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>& rhs )
	{
		lhs.swap(rhs);
	}
};

#include "SVReferenceCountingTemplate.inl"

#endif	//#ifndef SVREFERENCECOUNTINGTEMPLATE_H_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVReferenceCountingTemplate.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:46:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:08:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/