//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSet
//* .File Name       : $Workfile:   SVSet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:20  $
//******************************************************************************

#ifndef SVSET_H
#define SVSET_H

#include <set>

/**
@SVObjectName Seidenader Vision Inc. Set

@SVObjectOverview

@SVObjectOperations

*/
template<typename SVT_KEY>
class SVSet
{
public:
	typedef std::set< SVT_KEY > SVSetBase;
	typedef typename SVSetBase::key_type key_type;
	typedef typename SVSetBase::value_type value_type;
	typedef typename SVSetBase::size_type size_type;
	typedef typename SVSetBase::const_iterator const_iterator;
	typedef typename SVSetBase::iterator iterator;
	typedef typename SVSetBase::const_iterator const_iterator;
	typedef typename SVSetBase::iterator iterator;
	typedef typename SVSetBase::const_reference const_reference;
	typedef typename SVSetBase::reference reference;
	typedef std::pair< typename iterator, bool > SVInsertStatus;

	static SVSet Difference( const SVSet& p_SourceSet, const SVSet& p_FilterSet );

	SVSet();
	SVSet( const SVSet& p_rObject );
	SVSet( const SVSetBase& p_rObject );

	virtual ~SVSet();

	operator SVSetBase const & () const;
	operator SVSetBase & ();

	const SVSet& operator=( const SVSet& p_rObject );
	const SVSet& operator=( const SVSetBase& p_rObject );

	inline bool empty() const { return m_Set.empty(); }
	inline void clear() { m_Set.clear(); }

	inline size_type size() const { return m_Set.size(); }

	inline const_iterator begin() const { return m_Set.begin(); }
	inline const_iterator end() const { return m_Set.end(); }

	inline iterator begin() { return m_Set.begin(); }
	inline iterator end() { return m_Set.end(); }

	template< class _Iter >
	void insert( _Iter _First, _Iter _Last ) { m_Set.insert( _First, _Last ); }

	inline iterator insert( iterator p_Where, const value_type& p_rVal ) { return m_Set.insert( p_Where, p_rVal );}
	inline std::pair< iterator, bool > insert( const value_type& p_rValue ) { return m_Set.insert( p_rValue ); }

	inline size_type erase( const key_type& p_rKey ) { return m_Set.erase( p_rKey ); }
	inline iterator erase( iterator p_Iter ) { return m_Set.erase( p_Iter ); }

	// TSet Methods
	virtual INT_PTR GetSize() const;

	virtual INT_PTR Add( key_type newElement );
	virtual void Remove( key_type newElement );

	virtual key_type& GetAt( INT_PTR nIndex );

	virtual INT_PTR Find( key_type searchValue ) const;

protected:
	SVSetBase m_Set;

};

#include "SVSet.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVSet.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Sep 2011 12:51:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new insert method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Sep 2011 19:14:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new insert method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 11:49:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to add missing type identifiers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jun 2010 13:37:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:21:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

