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

#pragma once

//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <utility>

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

	~SVSet();

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
	INT_PTR GetSize() const;

	INT_PTR Add( key_type newElement );
	void Remove( key_type newElement );

	key_type& GetAt( INT_PTR nIndex );

	INT_PTR Find( key_type searchValue ) const;

protected:
	SVSetBase m_Set;

};

#include "SVSet.inl"

