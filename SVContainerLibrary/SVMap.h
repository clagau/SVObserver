//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMap
//* .File Name       : $Workfile:   SVMap.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:14  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>

template <typename SVT_KEY, typename SVT_VALUE>
class SVMap
{
public:
	typedef std::map< SVT_KEY, SVT_VALUE > SVMapBase;
	typedef typename SVMapBase::key_type key_type;
	typedef typename SVMapBase::mapped_type mapped_type;
	typedef typename SVMapBase::value_type value_type;
	typedef typename SVMapBase::size_type size_type;
	typedef typename SVMapBase::const_iterator const_iterator;
	typedef typename SVMapBase::iterator iterator;
	typedef typename SVMapBase::_Pairib SVIteratorBoolPair;

	SVMap();
	SVMap( const SVMap& p_rObject );
	SVMap( const SVMapBase& p_rObject );

	~SVMap();

	operator SVMapBase const & () const;
	operator SVMapBase & ();

	const SVMap& operator=( const SVMap& p_rObject );
	const SVMap& operator=( const SVMapBase& p_rObject );

	bool empty() const;

	void clear();

	size_type size() const;

	iterator begin();
	const_iterator begin() const;

	iterator end();
	const_iterator end() const;

	iterator find( const key_type& p_rKey );
	const_iterator find( const key_type& p_rKey ) const;

	SVIteratorBoolPair insert( const value_type& p_rKeyValuePair );
	SVIteratorBoolPair insert( const key_type& p_rKey, const mapped_type& p_rValue );

	template< class InputIterator >
	void insert( InputIterator p_First, InputIterator p_Last );

	size_type erase( const key_type& p_rKey );
	iterator erase( iterator p_Where );


	// MFC Style Operators
	bool IsEmpty() const;

	int GetCount() const;
	int GetSize() const;

	iterator GetStartPosition() const;

	void GetNextAssoc( iterator& rNextPosition, key_type& rKey, mapped_type& rValue ) const;

	bool Lookup( const key_type& key, mapped_type& rValue ) const;

	void SetAt( const key_type& key, mapped_type& newElement );

	mapped_type& operator[]( const key_type& key );

	void RemoveKey( const key_type& key );

	void RemoveAll();

protected:
	SVMapBase m_Map;

};

#include "SVMap.inl"
