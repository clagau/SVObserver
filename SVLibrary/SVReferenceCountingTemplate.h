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

