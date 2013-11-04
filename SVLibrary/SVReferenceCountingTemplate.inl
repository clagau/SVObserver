// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVReferenceCountingTemplate
// * .File Name       : $Workfile:   SVReferenceCountingTemplate.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:51:20  $
// ******************************************************************************

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
SVReferenceCountingTemplate()
{
	m_plRefCount = new ( Alloc(sizeof(long)) ) long;
	*m_plRefCount = 1;
	m_pResource = new ( Alloc(sizeof(RESOURCE)) ) RESOURCE;
	DefaultCreate( m_pResource );	// call RESOURCEMANAGER policy function
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
SVReferenceCountingTemplate( const this_type& rhs )
{
	Acquire( rhs );
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
SVReferenceCountingTemplate( RESOURCE& rhs )	// rhs is cleared
{
	m_plRefCount = new ( Alloc(sizeof(long)) ) long;
	*m_plRefCount = 1;
	m_pResource = new ( Alloc(sizeof(RESOURCE)) ) RESOURCE(rhs);
	Clear(rhs);
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline const SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>&
SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
operator = ( const this_type& rhs )
{
	if ( this != &rhs )
	{
		Release();
		Acquire( rhs );
	}
	return *this;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
~SVReferenceCountingTemplate()
{
	Release();
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline void SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
Acquire( const SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>& rhs )
{
	m_plRefCount = rhs.m_plRefCount;
	m_pResource = rhs.m_pResource;
	long lRefCount = ::InterlockedIncrement( m_plRefCount );
	ASSERT( lRefCount > 1 );
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline void SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
Release()
{
	long lRefCount = ::InterlockedDecrement( m_plRefCount );
	ASSERT( lRefCount >= 0 );
	if ( lRefCount == 0 )
	{
		Free( (const_cast<long*> (m_plRefCount)) );	// call ALLOCATIONPOLICY function
		Destroy(m_pResource);	// call RESOURCEMANAGER policy function
		Free( m_pResource );	// call ALLOCATIONPOLICY function
	}
	m_plRefCount = 0;
	m_pResource = NULL;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline RESOURCE* SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
Get()
{
	return m_pResource;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline RESOURCE& SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
GetRef()
{
	ASSERT( m_pResource );
	return *m_pResource;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline RESOURCE* SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
operator -> ()
{
	return m_pResource;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline const RESOURCE* SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
operator -> () const
{
	return m_pResource;
}

template < typename RESOURCE, typename RESOURCEMANAGER, typename ALLOCATIONPOLICY >
inline void SVReferenceCountingTemplate<RESOURCE, RESOURCEMANAGER, ALLOCATIONPOLICY>::
swap( this_type& rhs )
{
	if ( this != &rhs )
	{
		std::swap( m_plRefCount, rhs.m_plRefCount );
		std::swap( m_pResource, rhs.m_pResource );
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVReferenceCountingTemplate.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jun 2007 09:04:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jun 2007 15:12:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
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
